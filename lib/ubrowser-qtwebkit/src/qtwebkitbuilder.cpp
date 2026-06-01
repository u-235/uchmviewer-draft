
#include <QWebSettings>

#include <ubrowser/settings.hpp>
/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2026 Nick Egorrov, nicegorov@yandex.ru
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

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>

#include "qtwebkitbrowser.h"
#include "qtwebkitbuilder.h"


QString QtWebKitBuilder::kind() const
{
	return UBROWSER_KIND_HTML;
}

void QtWebKitBuilder::configure( const UBrowser::Settings& settings )
{
	// Disable caching, some pages not refreshing with caching
	QWebSettings::setObjectCacheCapacities( 0, 0, 0 );
	QWebSettings* setup = QWebSettings::globalSettings();

	setup->setAttribute( QWebSettings::AutoLoadImages, settings.enableImages );
	setup->setAttribute( QWebSettings::JavascriptEnabled, settings.enableJS );
	setup->setAttribute( QWebSettings::JavaEnabled, settings.enableJava );
	setup->setAttribute( QWebSettings::PluginsEnabled, settings.enablePlugins );
	setup->setAttribute( QWebSettings::OfflineStorageDatabaseEnabled, settings.enableOfflineStorage );
	setup->setAttribute( QWebSettings::LocalStorageDatabaseEnabled, settings.enableLocalStorage );
	setup->setAttribute( QWebSettings::LocalStorageEnabled, settings.enableLocalStorage );
}

UBrowser::Browser* QtWebKitBuilder::createBrowser(
    UBrowser::ContentProvider::Ptr contentProvider,
    QWidget* parentWidget,
    QObject* parent )
{
	return new QtWebKit::Browser( contentProvider, parentWidget, parent );
}

bool QtWebKitBuilder::hasOption( UBrowser::Option option ) const
{
	return QtWebKit::Browser::testOption( option );
}

bool QtWebKitBuilder::hasFeature( UBrowser::Feature feature ) const
{
	return QtWebKit::Browser::testFeature( feature );
}
