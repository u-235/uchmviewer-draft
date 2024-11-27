/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2022  Nick Egorrov, nicegorov@yandex.ru
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

#ifndef BROWSER_BUILDER_HPP
#define BROWSER_BUILDER_HPP

#include <QObject>
#include <QList>
#include <QtPlugin>

#include <browser/content-provider.hpp> // for ContentProvider, ContentProvider::Ptr
#include <browser/types.hpp>            // for Feature

class QDir;
class QString;


namespace Browser {

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

class Controller;


/**
 * @brief The Browser::Builder class provides a plugin interface for creating a
 * BrowserController instance.
 *
 * The application obtains an instance of the Browser::Controller class using the
 * createController() method. However, before the first call to the this method,
 * the application calls the setUrlSchemes() and setDataDirectory() methods.
 * The plugin should store this information and pass it to the created
 * BrowserController instances as needed.
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
class Builder : public QObject
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
		 * @brief Sets the URL schemas used in the application.
		 *
		 * The application uses various URL schemas, such as "ms-its" or "epub",
		 * which can be expanded to support new eBook formats. In general, the
		 * list of schemas becomes known when you start the program. At the same
		 * time some browsers (such as QtWebEngine) require registration of the
		 * URL schemas used.
		 *
		 * @param schemes
		 * @see QWebEngineUrlScheme
		 * @see QWebEngineUrlSchemeHandler
		 */
		virtual void setUrlSchemes(const QList<QString> schemes) = 0;

		/**
		 * @brief setDataDirectory
		 *
		 * @param dir
		 */
		virtual void setDataDirectory(const QDir dir) = 0;

		/**
		 * @brief createController
		 *
		 * @param contentProvider
		 * @param parent
		 * @return
		 */
		virtual Browser::Controller createController(
		    ContentProvider::Ptr contentProvider,
		    QObject* parent) = 0;

		/**
		 * @brief Checks the support for the setting option.
		 *
		 * @param[in] option The option whose support is being checked.
		 * @return Returns @c true if the option is supported.
		 * @see Browser::Controller::hasOption()
		 */
		virtual bool hasOption(Browser::Option option) const = 0;

		/**
		 * @brief Checks the support for the feature.
		 *
		 * @param[in] feature The feature whose support is being checked.
		 * @return Returns @c true if the feature is supported.
		 * @see Browser::Controller::hasFeature()
		 */
		virtual bool hasFeature(Browser::Feature feature) const = 0;

		/**
		 * @brief Brief description of the browser.
		 *
		 * A short description usually includes the name of the browser and the
		 * technology used. For example, "Default browser based on WebEngine".
		 */
		virtual QString brief() const = 0;

		/**
		 * @brief description of the browser.
		 */
		virtual QString description() const = 0;

		/**
		 * @brief license text.
		 */
		virtual QString license() const = 0;
};

} // namespace Browser

#define BROWSER_BUILDER_IID "ru.u-235.uChmViewer.BrowserBuilder/1.0"

Q_DECLARE_INTERFACE(Browser::Builder, BROWSER_BUILDER_IID)

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

#endif // BROWSER_BUILDER_HPP
