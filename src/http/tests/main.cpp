#include "config/Config.hpp"
#include "http/Context.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace config {

Config Config::instance;
bool Config::has_instance = false;

}

namespace {

static int g_failures = 0;

static bool enterProjectRoot(const char* executable) {
	char executable_path[PATH_MAX];
	char project_path[PATH_MAX];
	std::string tests_path;
	std::string::size_type slash;

	if (realpath(executable, executable_path) == NULL)
		return false;
	tests_path = executable_path;
	slash = tests_path.find_last_of('/');
	if (slash == std::string::npos)
		return false;
	tests_path.erase(slash);
	if (realpath((tests_path + "/../..").c_str(), project_path) == NULL)
		return false;
	return chdir(project_path) == 0;
}

static void fail(const std::string& test, const std::string& message) {
	std::cerr << "FAIL: " << test << ": " << message << std::endl;
	++g_failures;
}

static std::string readFile(const std::string& path) {
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	std::ostringstream content;

	if (!file)
		return "";
	content << file.rdbuf();
	return content.str();
}

static bool writeFile(const std::string& path, const std::string& content) {
	std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);

	if (!file)
		return false;
	file.write(content.data(), content.size());
	return file.good();
}

static bool fileExists(const std::string& path) {
	struct stat info;

	return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

static std::string httpFixture(const std::string& path) {
	const std::string source = readFile(path);
	const std::string::size_type separator = source.find("\n\n");
	std::string result;
	usize i;

	if (separator == std::string::npos)
		return source;
	i = 0;
	while (i < separator) {
		if (source[i] == '\n')
			result += "\r\n";
		else
			result += source[i];
		++i;
	}
	result += "\r\n\r\n";
	result += source.substr(separator + 2);
	return result;
}

static config::Config makeConfig() {
	config::Config config;
	config::LocationConfig root;
	config::LocationConfig upload;

	config.server.root = HTTP_SERVER_ROOT;
	config.server.client_max_body_size = 1024 * 1024;

	root.path = "/";
	root.root = HTTP_SERVER_ROOT;
	root.allowed_methods.insert("GET");
	root.allowed_methods.insert("DELETE");
	config.server.locations.push_back(root);

	upload.path = "/uploads";
	upload.root = HTTP_SERVER_ROOT;
	upload.allowed_methods.insert("POST");
	upload.upload_enabled = true;
	upload.upload_path = std::string(HTTP_SERVER_ROOT) + "/uploads";
	config.server.locations.push_back(upload);
	config.servers.push_back(config.server);
	return config;
}

static std::string exchange(const std::string& test,
		const std::string& request) {
	char output[32];
	std::vector<const config::ServerConfig*> servers;
	std::string response;
	usize steps = 0;

	servers.push_back(&config::Config::get_config().server);
	http::Context context(servers, 7, 11);
	if (context.consume(request.data(), request.size()) != request.size()) {
		fail(test, "Context did not consume the complete request");
		return "";
	}
	while (context.nextAction() == http::AC_WRITE && steps < 256) {
		usize produced = context.produce(output, sizeof(output));

		if (produced > sizeof(output)) {
			fail(test, "Context produced more bytes than the output buffer");
			break;
		}
		response.append(output, produced);
		++steps;
	}
	if (steps == 256)
		fail(test, "response generation exceeded the step limit");
	else if (context.nextAction() != http::AC_CLOSE)
		fail(test, "Context requested more input after receiving the full request");
	return response;
}

static bool responseMatches(const std::string& actual,
		const std::string& expected) {
	const std::string separator = "\r\n\r\n";
	std::string::size_type actual_end = actual.find(separator);
	std::string::size_type expected_end = expected.find(separator);
	std::string::size_type line_end;
	std::string::size_type cursor;

	if (actual_end == std::string::npos || expected_end == std::string::npos)
		return false;
	line_end = expected.find("\r\n");
	if (line_end == std::string::npos
		|| actual.compare(0, line_end, expected, 0, line_end) != 0)
		return false;
	cursor = line_end + 2;
	while (cursor < expected_end) {
		line_end = expected.find("\r\n", cursor);
		if (line_end == std::string::npos || line_end > expected_end)
			line_end = expected_end;
		if (line_end != cursor) {
			const std::string header = expected.substr(cursor, line_end - cursor);
			if (actual.substr(0, actual_end + 2).find(header + "\r\n")
				== std::string::npos)
				return false;
		}
		cursor = line_end + 2;
	}
	return actual.substr(actual_end + separator.size())
		== expected.substr(expected_end + separator.size());
}

static void checkResponse(const std::string& test,
		const std::string& response) {
	const std::string expected = httpFixture(
		"http/tests/responses/" + test + ".http");

	if (!responseMatches(response, expected)) {
		fail(test, "response does not match the expected response file");
		return ;
	}
	std::cout << "PASS: " << test << std::endl;
}

static void testGet() {
	const std::string request = httpFixture("http/tests/requests/get.http");

	checkResponse("get", exchange("get", request));
}

static void testPost() {
	const std::string output = std::string(HTTP_SERVER_ROOT)
		+ "/uploads/post.txt";
	const std::string request = httpFixture("http/tests/requests/post.http");
	const std::string response = exchange("post", request);

	checkResponse("post", response);
	if (readFile(output) != "uploaded content\n")
		fail("post", "uploaded file content is incorrect");
	std::remove(output.c_str());
}

static void testPostConflict() {
	const std::string output = std::string(HTTP_SERVER_ROOT)
		+ "/uploads/post.txt";
	const std::string original = "existing content\n";
	const std::string request = httpFixture("http/tests/requests/post.http");

	if (!writeFile(output, original)) {
		fail("post_conflict", "could not create the existing upload");
		return ;
	}
	checkResponse("post_conflict", exchange("post_conflict", request));
	if (readFile(output) != original)
		fail("post_conflict", "existing upload was modified");
	std::remove(output.c_str());
}

static void testMethodNotAllowed() {
	const std::string request = httpFixture(
		"http/tests/requests/method_not_allowed.http");

	checkResponse("method_not_allowed",
		exchange("method_not_allowed", request));
}

static void testLengthRequired() {
	const std::string request = httpFixture(
		"http/tests/requests/length_required.http");

	checkResponse("length_required", exchange("length_required", request));
}

static void testDelete() {
	const std::string target = std::string(HTTP_SERVER_ROOT)
		+ "/delete.txt";
	const std::string body = readFile("http/tests/requests/delete.body");
	const std::string request = httpFixture("http/tests/requests/delete.http");

	if (!writeFile(target, body)) {
		fail("delete", "could not create the target file");
		return ;
	}
	checkResponse("delete", exchange("delete", request));
	if (fileExists(target))
		fail("delete", "target file still exists after DELETE");
	std::remove(target.c_str());
}

}

int main(int argc, char** argv) {
	if (argc == 0 || !enterProjectRoot(argv[0])) {
		std::cerr << "FAIL: could not locate the project root" << std::endl;
		return 1;
	}
	config::Config config = makeConfig();

	config::Config::set_config(config);
	testGet();
	testPost();
	testPostConflict();
	testMethodNotAllowed();
	testLengthRequired();
	testDelete();
	if (g_failures != 0) {
		std::cerr << g_failures << " HTTP Context test(s) failed" << std::endl;
		return 1;
	}
	std::cout << "All HTTP Context tests passed" << std::endl;
	return 0;
}
