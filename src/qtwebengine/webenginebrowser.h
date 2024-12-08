/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2016 George Yunaev, gyunaev@ulduzsoft.com
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
#include <QUrl>
#include <QtGlobal>

class QMenu;
class QPrinter;
class QWidget;

#include <ubrowser/browser.hpp>
#include <ubrowser/types.hpp>

namespace UBrowser
{
struct Settings;
}

class WebEngineWidget;


class WebEngineBrowser : public UBrowser::Browser
{
		Q_OBJECT

	public:
		WebEngineBrowser( UBrowser::ContentProvider::Ptr contentProvider, QObject* parent );
		virtual ~WebEngineBrowser();

		QWidget* view() override;

		bool hasOption( UBrowser::Option option ) override;

		bool hasFeature( UBrowser::Feature feature ) const override;

		//! Open a page from current chm archive
		void load( const QUrl& url ) override;

		QUrl url() const override;

		void reload() override;

	public:
		// Apply the configuration settings (JS enabled etc) to the web renderer
		static  void applySettings( UBrowser::Settings& settings );

		//! Invalidate current view, doing all the cleanups etc.
		void invalidate() override;

		//! Prints the current page on the printer.
		void print( QPrinter* printer, std::function<void ( bool success )> result ) override;

		//! Return current ZoomFactor.
		qreal zoomFactor() const override;

		//! Sets ZoomFactor. The value returned by getZoomFactor(), given to this function, should give the same result.
		void setZoomFactor( qreal zoom ) override;

		void zoomIncrease() override;

		void zoomDecrease() override;

		/*!
		* Return current scrollbar position in view window. Saved on program exit.
		* There is no restriction on returned value, except that giving this value to
		* setScrollbarPosition() should move the scrollbar in the same position.
		*/
		int scrollTop() override;

		//! Sets the scrollbar position.
		void setScrollTop( int pos ) override;

		void setAutoScroll( int pos ) override;

		void findText( const QString& text,
		               bool backward,
		               bool caseSensitively,
		               bool highlightSearchResults,
		               std::function<void ( bool found, bool wrapped )> result ) override;

		//! Select the content of the whole page
		void selectAll() override;

		//! Copies the selected content to the clipboard
		void selectedCopy() override;

		QString selectedText() const override;

		//! Returns the window title
		QString title() const override;

		bool canGoBack() const override;

		bool canGoForward() const override;

		void back() override;

		void forward() override;

		void configure( const UBrowser::Settings& settings ) override;

		UBrowser::History* history() const override;

	protected:
		bool openPage( const QUrl& url );
		void handleStartPageAsImage( QUrl& link );

		// Overriden to change the source
		void setSource( const QUrl& name );

	private slots:
		void onLoadFinished( bool success );
		void onUrlChanged( const QUrl& url );
		void onLinkClicked( const QUrl& link, UBrowser::OpenMode mode );
		void onContextMenuRequested( const QPoint& globalPos, const QUrl& link );

	private:
		WebEngineWidget* m_widget;

		// Keeps the scrollbar position to move after the page is loaded
		int m_storedScrollbarPosition;
};

#endif // QTWEBENGINE_VIEWWINDOW_H
