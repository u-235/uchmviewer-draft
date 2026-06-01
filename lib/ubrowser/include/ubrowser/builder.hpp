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

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>

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
		 * Currently there is only the "html" kind; there may be "txt" and
		 * "rtf" types.
		 *
		 * @return A string with the browser kind.
		 */
		virtual QString kind() const = 0;

		/**
		 * @brief Configures the browser.
		 *
		 * The browser may not support some of the options available in the
		 * UBrowser::Settings structure. Use the hasOption() method to check for
		 * supported options.
		 *
		 * @param[in] settings The settings that apply.
		 */
		virtual void configure( const UBrowser::Settings& settings ) = 0;

		/**
		 * @brief createBrowser
		 *
		 * @param contentProvider
		 * @param parent
		 * @return
		 */
		virtual UBrowser::Browser* createBrowser(
		    ContentProvider::Ptr contentProvider,
		    QWidget* parentWidget,
		    QObject* parent ) = 0;

		/**
		 * @brief Checks the support for the setting option.
		 *
		 * @param[in] option The option whose support is being checked.
		 * @return Returns @c true if the option is supported.
		 * @see UBrowser::Browser::hasOption()
		 */
		virtual bool hasOption( UBrowser::Option option ) const = 0;

		/**
		 * @brief Checks the support for the feature.
		 *
		 * @param[in] feature The feature whose support is being checked.
		 * @return Returns @c true if the feature is supported.
		 * @see UBrowser::Browser::hasFeature()
		 */
		virtual bool hasFeature( UBrowser::Feature feature ) const = 0;
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#define UBROWSER_BUILDER_IID "org.uChmViewer.UBrowserBuilder"
#define UBROWSER_BUILDER_HTML_IID UBROWSER_BUILDER_IID "." UBROWSER_KIND_HTML
#define UBROWSER_BUILDER_RTF_IID  UBROWSER_BUILDER_IID "." UBROWSER_KIND_RTF
#define UBROWSER_BUILDER_TXT_IID  UBROWSER_BUILDER_IID "." UBROWSER_KIND_TXT

Q_DECLARE_INTERFACE( UBrowser::Builder, UBROWSER_BUILDER_IID )

#endif // UBROWSER_BUILDER_HPP
