#pragma once 

namespace http {
	
	struct BodyContext;
	
	class IRequestHandler {
		
		public:
			virtual ~IRequestHandler() {}
			virtual void handle() = 0;
			virtual bool done() = 0;
			virtual BodyContext create_body_context() = 0;
	};
}



