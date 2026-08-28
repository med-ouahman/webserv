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
	void add(std::string const& name, std::string const& value);
	Headers();
	Headers(const Headers& other);
	~Headers();

	typedef std::vector<Header>::const_iterator const_iterator;

	const_iterator begin() const;
	const_iterator end() const;
};

}
