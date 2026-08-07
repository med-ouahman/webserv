
#include "http/pipeline/handlers/UploadHandler.hpp"
#include "http/Context.hpp"
#include "http/limits.hpp"
#include "Logger.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <sstream>

namespace http {

namespace {

static std::string pathJoin(const std::string& left,
		const std::string& right) {
	if (left.empty() || left[left.size() - 1] == '/')
		return left + right;
	return left + "/" + right;
}

static std::string basenameOf(const std::string& path) {
	std::string::size_type slash = path.find_last_of('/');

	if (slash == std::string::npos)
		return path.empty() ? "upload.bin" : path;
	if (slash + 1 >= path.size())
		return "upload.bin";
	return path.substr(slash + 1);
}

static std::string uploadLocation(const std::string& normalized_path,
		const std::string& filename) {
	std::string::size_type slash = normalized_path.find_last_of('/');

	if (slash == std::string::npos)
		return "/" + filename;
	return normalized_path.substr(0, slash + 1) + filename;
}

static bool pathExists(const std::string& path) {
	struct stat info;

	return stat(path.c_str(), &info) == 0;
}

static bool validUploadDirectory(const std::string& path) {
	struct stat info;

	return stat(path.c_str(), &info) == 0
		&& S_ISDIR(info.st_mode)
		&& access(path.c_str(), W_OK) == 0;
}

static Error copyBody(base::io::Reader& reader, const std::string& path) {
	char read_buffer[limits::BODY_BUFFER_SIZE];
	char write_buffer[limits::BODY_BUFFER_SIZE];
	base::io::Writer writer;

	if (!writer.reset(path, write_buffer, limits::BODY_BUFFER_SIZE))
		return ERR_INTERNAL;
	if (reader.type() == base::io::Reader::NONE)
		return ERR_NONE;
	while (true) {
		base::Expected<usize, base::io::Error> chunk =
			reader.read(read_buffer, limits::BODY_BUFFER_SIZE);
		if (!chunk)
			return ERR_INTERNAL;
		if (chunk.value() == 0)
			break;
		base::Expected<usize, base::io::Error> written =
			writer.write(read_buffer, chunk.value());
		if (!written || written.value() != chunk.value())
			return ERR_INTERNAL;
	}
	if (!writer.flush())
		return ERR_INTERNAL;
	return ERR_NONE;
}

static Error putBody(base::io::Reader& reader, const std::string& path) {
	if (reader.type() == base::io::Reader::FILE) {
		if (std::rename(reader.path().c_str(), path.c_str()) == 0)
				return ERR_NONE;
		if (errno != EXDEV)
				return ERR_INTERNAL;
	}
	Error err = copyBody(reader, path);

	if (err != ERR_NONE)
		return err;
	if (reader.type() == base::io::Reader::FILE && !reader.path().empty())
		std::remove(reader.path().c_str());
	return ERR_NONE;
}

}

UploadHandler::UploadHandler(Context& context)
	: ARequestHandler(context) {}

UploadHandler::~UploadHandler() {}

Error UploadHandler::handle() {
	Error err;
	std::string filename;
	const std::string& dir = decision().upload_path;
	std::string path;

	if (request().has_body and request().body.type() == base::io::Reader::NONE) {
		context_.action_ = AC_READ;
		context_.services_.logger.log(logger::Debug,
			"http: upload waiting for more body", false);
		return ERR_NONE;
	}
	if (!validUploadDirectory(dir)) return ERR_INTERNAL;
	filename = basenameOf(decision().normalized_path);
	path = pathJoin(dir, filename);
	if (pathExists(path))
		return ERR_CONFLICT;
	TRY(putBody(request().body, path), err);
	setStatus(CREATED);
	setBodyFixed("");
	setHeader("Location", uploadLocation(decision().normalized_path, filename));
	setContentLength();
	setConnection();
	setDate();
	{
		std::ostringstream ss;

		ss << "http: upload completed -> " << path;
		context_.services_.logger.log(logger::Debug, ss.str(), false);
	}
	responseReady();
	return ERR_NONE;
}

}
