#pragma once

#include <string>
#include <vector>

namespace http {
class Headers {

private:
	struct Header {
		std::string name;
		std::string value;
		Header(const std::string& _name, const std::string& _value): name(_name), value(_value) {}
	};
	
	std::vector<Header> headers_;

public:
	const std::string& get(const std::string& name) const;
	const std::string& operator[](const std::string& name);
	void add(std::string const& name, std::string const& value);
	void remove(std::string const& name);
	bool has(const std::string& name);
	bool replace(std::string const& name, std::string const& new_value);
	Headers();
	Headers(const Headers& other);
	~Headers();
	Headers& operator=(const Headers& other);
	size_t size() const;

	typedef std::vector<Header>::iterator iterator;
	typedef std::vector<Header>::const_iterator const_iterator;

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
};

}
