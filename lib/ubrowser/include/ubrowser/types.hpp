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

#ifndef UBROWSER_TYPES_HPP
#define UBROWSER_TYPES_HPP


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief The Feature enumeration is used in the UBrowser::Browser::hasFeature()
 * method.
 */
enum Feature
{
	/// The browser supports @ref browser-feature-copytext
	/// "text selection and copying".
	FEATURE_COPY,
	/// The browser supports @ref browser-feature-findtext
	/// "searching in the text" of the page.
	FEATURE_SEARCH,
	/// The browser supports @ref browser-feature-history
	/// "the navigation history".
	FEATURE_HISTORY,
	/// The browser supports @ref browser-feature-print "the page printing".
	FEATURE_PRINT,
	/// The browser supports @ref browser-feature-scroll "the page scrolling".
	FEATURE_SCROLL,
	/// The browser supports @ref browser-feature-zoom "the page zooming".
	FEATURE_ZOOM
};

/**
 * @brief The Options enumeration is used in the UBrowser::Browser::hasOption()
 * method.
 */
enum Option
{
	/// The browser supports settings for highlighting search results.
	OPTION_HIGH_LIGHT_SEARCH_RESULT,
	/// The browser supports image display disabled/enabled.
	OPTION_IMAGES,
	/// The browser supports Java enable/disable.
	OPTION_JAVA,
	/// The browser supports JavaScript enable/disable.
	OPTION_JAVA_SCRIPT,
	/// The browser supports local storage enable/disable.
	OPTION_LOCAL_STORAGE,
	/// The browser supports offline storage enable/disable.
	OPTION_OFFLINE_STORAGE,
	/// The browser supports plugins enable/disable.
	OPTION_PLUGINS
};

/**
 * @brief The OpenMode enumeration used in the UBrowser::Browser::linkClicked()
 * signal.
 */
enum OpenMode
{
	/// The link should open in the current tab.
	OPEN_IN_CURRENT,
	/// The link should open in the new foregraund tab.
	OPEN_IN_NEW,
	/// The link should open in the new backgraund tab.
	OPEN_IN_BACKGROUND
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_TYPES_HPP
