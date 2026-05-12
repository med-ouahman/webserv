#pragma once 

namespace http {
	
	class IRequestHandler {
		
		public:
			virtual ~IRequestHandler() {}
			virtual void handle( core::Connection& ) = 0;
			virtual bool done() = 0;
	};
}



