#pragma once

namespace core {
	
	struct ConnectionAction {
		
		public:
			enum Type {
				READ,
				WRITE,
				DISABLE_READ,
				DISABLE_WRITE,
				CLOSE,
			};
	};
}
