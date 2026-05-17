
#pragma once

namespace Base {

template<typename T>
struct Optional {
	T		value;
	bool	exist;

	Optional() : exist(false) {}
	Optional(T val) : value(val), exist(true) {}

	bool has_value() const { return exist; };
};

}
