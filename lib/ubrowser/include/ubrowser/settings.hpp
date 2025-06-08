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

#ifndef UBROWSER_SETTINGS_HPP
#define UBROWSER_SETTINGS_HPP


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * The structure holds the settings for the Browser::Controller.
 */
struct Settings
{
	bool enableJS;
	bool enableJava;
	bool enablePlugins;
	bool enableImages;
	bool enableOfflineStorage;
	bool enableLocalStorage;
	bool highlightSearchResults;
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_SETTINGS_HPP
