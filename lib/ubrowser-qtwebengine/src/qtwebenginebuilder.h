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

#ifndef QTWEBENGINEBUILDER_H
#define QTWEBENGINEBUILDER_H

#include <QObject>
#include <QString>

#include <ubrowser/builder.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/export.hpp>


class UBROWSER_API_DECL QtWebEngineBuilder: public QObject, public UBrowser::Builder
{
		Q_OBJECT
		Q_PLUGIN_METADATA( IID UBROWSER_BUILDER_HTML_IID FILE "qtwebengine.json" )
		Q_INTERFACES( UBrowser::Builder )

	public:
		QString kind() const override;
		void configure( const UBrowser::Settings& settings ) override;
		UBrowser::Browser* createBrowser(
		    UBrowser::ContentProvider::Ptr contentProvider,
		    QWidget* parentWidget,
		    QObject* parent ) override;
		bool hasOption( UBrowser::Option option ) const override;
		bool hasFeature( UBrowser::Feature feature ) const override;
};

#endif // QTWEBENGINEBUILDER_H
