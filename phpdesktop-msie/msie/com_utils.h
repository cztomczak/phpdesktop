// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"

// Include before comdef.h so that smart pointer types are defined.
#include "MsHTML.h"

#include <comdef.h>
/*
These are already included in comdef.h:
#include <comip.h> // _com_ptr_t, _COM_SMARTPTR_TYPEDEF
#include <comdefsp.h> // smart pointer defs, IDispatch <> IDispatchPtr
#include <comutil.h> // _bstr_t, _variant_t
*/

// #include <atlcomcli.h> // CComBSTR, CComPtr, CComQIPtr

/*
Already defined in comdef.h:
#ifdef _DEBUG
 #pragma comment(lib, "comsuppwd.lib")
#else
 #pragma comment(lib, "comsuppw.lib")
#endif // _DEBUG
*/

// Some smart pointer defs are missing.
_COM_SMARTPTR_TYPEDEF(IHTMLDocument3, IID_IHTMLDocument3);
_COM_SMARTPTR_TYPEDEF(IHTMLElement2, IID_IHTMLElement2);
