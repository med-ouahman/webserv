
template<typename T>
struct Optional {
	T		value;
	bool	exists;

	Optional() : exists(false) {}
	Optional(T v) : value(v), exists(true) {}

	bool has_value() const { return exists; }
};

namespace http {

enum Method { GET, POST, DELETE };
enum { file, directory, executable, not_found };

class Context {

	Method					method;
	std::string				path;
	Optional<usize>			content_length;
	Reader					body;
	Optional<std::string>	query;

	Optional<std::string>	root_dir;
	std::string				cgi_dir;
	usize					UploadSizeLimit;
	usize					BufferThreshold;

	PathType				path_type;
	Optional<usize>			file_size;
};

}
