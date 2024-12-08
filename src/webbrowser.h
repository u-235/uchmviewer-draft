#ifndef WEBBROWSER_H
#define WEBBROWSER_H

// We support both engines
#if defined (USE_WEBENGINE)
	#include "qtwebengine/webenginebrowser.h"
	typedef WebEngineBrowser WebBrowser;
#else
	#include "qtwebkit/webkitbrowser.h"
	typedef WebKitBrowser WebBrowser;
#endif

#endif // WEBBROWSER_H
