#pragma once

#include <ctime>


class Timestamp {
	
	private:
		time_t seconds_;
		Timestamp( const Timestamp& );
		Timestamp& operator=( const Timestamp& );
		
	public:
		explicit Timestamp(): seconds_(::time(NULL)) {}
		explicit Timestamp(int): seconds_(0){}
		static Timestamp now() {
			return Timestamp();
		};

		void update() {
			seconds_ = ::time(NULL);
		};

		double elapsed() const {
			return ::difftime(::time(NULL), seconds_);
		};

		time_t seconds() const {
			return seconds_;
		};

		bool operator<( const Timestamp& ts ) const {
			return seconds_ < ts.seconds_;
		}
		
		bool operator>( const Timestamp& ts ) const {
			return seconds_ > ts.seconds_;
		}

		bool operator==( const Timestamp& ts ) const {
			return seconds_ == ts.seconds_;
		}

		bool operator>=( const Timestamp& ts ) const {
			return seconds_ >= ts.seconds_;
		}

		bool operator<=( const Timestamp& ts ) const {
			return seconds_ <= ts.seconds_;
		}

		bool operator not_eq( const Timestamp& ts ) const {
			return seconds_ not_eq ts.seconds_;
		}

};

