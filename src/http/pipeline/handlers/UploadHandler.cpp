
#include "http/pipeline/handlers/UploadHandler.hpp"
#include "http/Context.hpp"
#include "http/limits.hpp"

#include <sys/stat.h>
#include <unistd.h>

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

}

UploadHandler::UploadHandler(Context& context)
	: RequestHandler(context) {}

UploadHandler::~UploadHandler() {}

Error UploadHandler::handle() {
	Error err;
	std::string filename;
	std::string path;

	if (!validUploadDirectory(*decision().upload_path))
		return ERR_INTERNAL;
	filename = basenameOf(decision().normalized_path);
	path = pathJoin(*decision().upload_path, filename);
	if (pathExists(path))
		return ERR_METHOD_NOT_ALLOWED;
	TRY(copyBody(request().body, path), err);
	setStatus(CREATED);
	setBodyFixed("");
	setHeader("Location", path);
	setContentLength();
	setConnection();
	setDate();
	return ERR_NONE;
}

}
