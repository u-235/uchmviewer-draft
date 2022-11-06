/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2022-2025 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UBROWSER_API_EXPORT_HPP
#define UBROWSER_API_EXPORT_HPP

#include <QtCore/QtGlobal>


//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
  * #BROWSER_API_DECL macro for declaring an API symbol exported by the library
  * or imported by client code.
  *
  * @see https://learn.microsoft.com/en-us/cpp/cpp/dllexport-dllimport
  * @see https://doc.qt.io/qt-5/sharedlibrary.html
  */
#ifndef UBROWSER_API_STATIC
	#ifdef UBROWSER_API_EXPORT
		#define UBROWSER_API_DECL Q_DECL_EXPORT
	#else
		#define UBROWSER_API_DECL Q_DECL_IMPORT
	#endif
#else
	#define UBROWSER_API_DECL
#endif

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

#endif // UBROWSER_API_EXPORT_HPP
