#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

// We support both engines
#if defined (USE_WEBENGINE)
	#include "qtwebengine/webenginecontroller.h" // IWYU pragma: export
	typedef WebEngineController WebController;
#else
	#include "qtwebkit/webkitcontroller.h"    // IWYU pragma: export
	typedef WebKitController WebController;
#endif

#endif // VIEWWINDOW_H
