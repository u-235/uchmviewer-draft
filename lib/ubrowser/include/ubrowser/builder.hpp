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

#ifndef UBROWSER_BUILDER_HPP
#define UBROWSER_BUILDER_HPP

#include <QObject>
#include <QList>
#include <QtPlugin>

#include <ubrowser/contentprovider.hpp>
#include <ubrowser/types.hpp>

class QDir;
class QString;


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

class Browser;
struct Settings;


/**
 * @brief The UBrowser::Builder class provides a plugin interface for creating a
 * UBrowser::Browser instance.
 *
 * The application obtains an instance of the UBrowser::Browser class using the
 * createController() method. However, before the first call to the this method,
 * the application calls the setDataDirectory() method. The plugin should store
 * this information and pass it to the created UBrowser::Browser instances as
 * needed.
 *
 *
 * <h2>Information about plugin</h2>
 *
 * The kind(), hasOption() and hasFeature() methods inform the application about
 * the type of browser and supported options and features.
 *
 * The application uses the brief(), description(), and license() methods to
 * inform the user about the browser. These methods should return plain text.
 * The application, when displaying this text, removes single newline characters
 * with no space preceding them. Two newline characters result in a new
 * paragraph. This is similar to Markdown and is the only formatting available.
 * Information in English is mandatory. Information in other languages
 * (according to the locale of the application) is welcome.
 *
 * @see https://doc.qt.io/qt-5/plugins-howto.html
 */
class Builder
{
	public:
		virtual ~Builder() {}

		/**
		 * @brief Returns the browser kind.
		 *
		 * Currently there is only the "browser/html" kind; there may be
		 * "browser/txt" and "browser/rtf" types.
		 *
		 * @return A string with the browser kind.
		 */
		virtual QString kind() const = 0;

		/**
		 * @brief Configures the browser.
		 *
		 * The browser may not support some of the options available in the
		 * BrowserSettings structure. Use the hasOption() method to check for
		 * supported options.
		 *
		 * @param[in] settings The settings that apply.
		 */
		virtual void configure( const UBrowser::Settings& settings ) = 0;

		/**
		 * @brief setDataDirectory
		 *
		 * @param dir
		 */
		virtual void setDataDirectory( const QDir& dir ) = 0;

		/**
		 * @brief createController
		 *
		 * @param contentProvider
		 * @param parent
		 * @return
		 */
		virtual UBrowser::Browser* createController(
		    ContentProvider::Ptr contentProvider,
		    QObject* parent ) = 0;

		/**
		 * @brief Checks the support for the setting option.
		 *
		 * @param[in] option The option whose support is being checked.
		 * @return Returns @c true if the option is supported.
		 * @see UBrowser::Controller::hasOption()
		 */
		virtual bool hasOption( UBrowser::Option option ) const = 0;

		/**
		 * @brief Checks the support for the feature.
		 *
		 * @param[in] feature The feature whose support is being checked.
		 * @return Returns @c true if the feature is supported.
		 * @see UBrowser::Controller::hasFeature()
		 */
		virtual bool hasFeature( UBrowser::Feature feature ) const = 0;
};

} // namespace UBrowser

#define UBROWSER_BUILDER_IID "ru.u-235.uChmViewer.UBrowserBuilder/1.0"

Q_DECLARE_INTERFACE( UBrowser::Builder, UBROWSER_BUILDER_IID )

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

#endif // UBROWSER_BUILDER_HPP
