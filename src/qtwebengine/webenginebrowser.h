/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2016 George Yunaev, gyunaev@ulduzsoft.com
 *  Copyright (C) 2025 Nick Egorrov, nicegorov@yandex.ru
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

#ifndef QTWEBENGINE_VIEWWINDOW_H
#define QTWEBENGINE_VIEWWINDOW_H

#include <functional>

#include <QObject>
#include <QString>
#include <QtGlobal>

class QPrinter;
class QUrl;
class QWidget;

#include <ubrowser/abstractbrowser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/types.hpp>

namespace UBrowser
{
struct Settings;
}

class WebEngineWidget;


class WebEngineBrowser : public UBrowser::AbstractBrowser
{
		Q_OBJECT

	public:
		WebEngineBrowser( UBrowser::ContentProvider::Ptr content, QObject* parent );
		virtual ~WebEngineBrowser();

		// Apply the configuration settings (JS enabled etc) to the web renderer
		static  void applySettings( UBrowser::Settings& settings );

		QWidget* view() override;
		bool hasOption( UBrowser::Option option ) override;
		bool hasFeature( UBrowser::Feature feature ) const override;
		void print( QPrinter* printer, std::function<void ( bool success )> result ) override;
		qreal zoomFactor() const override;
		void setScrollTop( int pos ) override;
		void findText( const QString& text,
		               bool backward,
		               bool caseSensitively,
		               bool highlightSearchResults,
		               std::function<void ( bool found, bool wrapped )> result ) override;
		void selectAll() override;
		QString selectedText() const override;
		QString title() const override;

	protected:
		void injectJS() override;
		void loadImpl( const QUrl& url ) override;
		void setZoomFactorImpl( qreal zoom ) override;

	private:
		WebEngineWidget* m_widget;
};

#endif // QTWEBENGINE_VIEWWINDOW_H
