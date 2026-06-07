
namespace http {

class IHandler {
	public:
		virtual void handle( Context& ctx, Writer& w) = 0;
		virtual ~IHandler() {}
};

}
