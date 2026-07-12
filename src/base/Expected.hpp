
#pragma once

#include <new>

namespace base {

/*
 * in order to for correct alignment we used align_
 *
 * */
template<typename T, typename E>
class Expected {
private:
	union Storage {
		char		t_buf[sizeof(T)];
		long double	align_;
		void*		ptr_;
	} storage;

	int err_code;

private:
	T*			val_ptr()		{ return reinterpret_cast<T*>(storage.t_buf); }
	const T*	val_ptr() const	{ return reinterpret_cast<const T*>(storage.t_buf); }

	void destroy() {
		if (err_code == 0)
			val_ptr()->~T();
	}

public:
	Expected( const T& val ) : err_code(0) {
		new (val_ptr()) T(val);
	}

	Expected( E err ) : err_code(static_cast<int>(err)) {}

	Expected( const Expected& other ) : err_code(other.err_code) {
		if (err_code == 0)
			new (val_ptr()) T(*other.val_ptr());
	}

	Expected& operator=( const Expected& other ) {
		if (this != &other) {
			destroy();
			err_code = other.err_code;
			if (err_code == 0)
				new (val_ptr()) T(*other.val_ptr());
		}
		return *this;
	}

	~Expected() { destroy(); }

	bool		has_value()		const	{ return err_code == 0; }
	operator	bool()			const	{ return err_code == 0; }

	T&			value()					{ return *val_ptr(); }
	const T&	value()			const	{ return *val_ptr(); }
	T&			operator*()				{ return *val_ptr(); }
	const T&	operator*()		const	{ return *val_ptr(); }
	T*			operator->()			{ return val_ptr(); }
	const T*	operator->()	const	{ return val_ptr(); }

	E			error()			const	{ return static_cast<E>(err_code); }
};

}
