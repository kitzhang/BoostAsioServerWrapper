#ifndef session_interface_h__
#define session_interface_h__

#include "lib_export.h"

LIB_API class session_interface{
public:
	virtual ~session_interface(void){};
	virtual void send();
};

#endif

