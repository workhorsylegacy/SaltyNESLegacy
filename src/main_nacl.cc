
#ifdef NACL

#include <ppapi/cpp/module.h>

#include "nes_cpp.h"

// The Module class.  The browser calls the CreateInstance() method to create
// an instance of your NaCl module on the web page.  The browser creates a new
// instance for each <embed> tag with type="application/x-nacl".
class NaclNesModule : public pp::Module {
	public:
	NaclNesModule() : pp::Module() {}
	virtual ~NaclNesModule() {}

	// Create and return a NaclNesInstance object.
	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new NaclNes(instance);
	}
};

// Factory function called by the browser when the module is first loaded.
// The browser keeps a singleton of this module.  It calls the
// CreateInstance() method on the object you return to make instances.  There
// is one instance per <embed> tag on the page.  This is the main binding
// point for your NaCl module with the browser.
namespace pp {
	Module* CreateModule() {
		return new NaclNesModule();
	}
}

#endif
