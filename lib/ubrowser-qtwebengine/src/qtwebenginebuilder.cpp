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

#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/settings.hpp>

#include "qtwebenginebrowser.h"
#include "qtwebenginebuilder.h"


QString QtWebEngineBuilder::kind() const
{
	return UBROWSER_KIND_HTML;
}

void QtWebEngineBuilder::configure( const UBrowser::Settings& settings )
{
	QWebEngineSettings* setup = QWebEngineProfile::defaultProfile()->settings();

	setup->setAttribute( QWebEngineSettings::AutoLoadImages, settings.enableImages );
	setup->setAttribute( QWebEngineSettings::JavascriptEnabled, settings.enableJS );
	setup->setAttribute( QWebEngineSettings::PluginsEnabled, settings.enablePlugins );
	setup->setAttribute( QWebEngineSettings::LocalStorageEnabled, settings.enableLocalStorage );
}

UBrowser::Browser* QtWebEngineBuilder::createBrowser(
    UBrowser::ContentProvider::Ptr contentProvider,
    QObject* parent )
{
	return new QtWebEngine::Browser( contentProvider, parent );
}

bool QtWebEngineBuilder::hasOption( UBrowser::Option option ) const
{
	return QtWebEngine::Browser::testOption( option );
}

bool QtWebEngineBuilder::hasFeature( UBrowser::Feature feature ) const
{
	return QtWebEngine::Browser::testFeature( feature );
}
