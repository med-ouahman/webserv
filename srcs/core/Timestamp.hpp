#pragma once

#include <ctime>

namespace core {

	class Timestamp {
		
		private:
			time_t seconds_;
			Timestamp( const Timestamp& );
			Timestamp& operator=( const Timestamp& );
			
		public:
			explicit Timestamp(): seconds_(::time(NULL)) {}
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
			}

	};

}
