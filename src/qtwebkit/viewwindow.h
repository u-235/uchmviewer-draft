/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2014 George Yunaev, gyunaev@ulduzsoft.com
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

#ifndef VIEWWINDOW_WEBKIT_H
#define VIEWWINDOW_WEBKIT_H

#include <functional>

#include <QObject>
#include <QString>
#include <QUrl>
#include <QWebView>
#include <QtGlobal>

class QContextMenuEvent;
class QMouseEvent;
class QPoint;
class QPrinter;
class QWidget;

#include <ubrowser/types.hpp>

namespace UBrowser
{
struct Settings;
}


class ViewWindow : public QWebView
{
		Q_OBJECT

	public:
		ViewWindow( QWidget* parent );
		virtual ~ViewWindow();

		//! Open a page from current chm archive
		void    load( const QUrl& url );

		QUrl    url() const   { return QWebView::url(); }

	signals:
		void    dataLoaded( ViewWindow* window );
		void    linkClicked( const QUrl& link, UBrowser::OpenMode mode );
		void    contextMenuRequested( const QPoint& globalPos, const QUrl& url );

	public:
		// Apply the configuration settings (JS enabled etc) to the web renderer
		static  void    applySettings( UBrowser::Settings& settings );

		//! Invalidate current view, doing all the cleanups etc.
		void    invalidate();

		//! Prints the current page on the printer.
		void print( QPrinter* printer, std::function<void ( bool success )> result );

		//! Return current ZoomFactor.
		qreal   zoomFactor() const;

		//! Sets ZoomFactor. The value returned by getZoomFactor(), given to this function, should give the same result.
		void    setZoomFactor( qreal zoom );

		/*!
		* Return current scrollbar position in view window. Saved on program exit.
		* There is no restriction on returned value, except that giving this value to
		* setScrollbarPosition() should move the scrollbar in the same position.
		*/
		int     scrollTop();

		//! Sets the scrollbar position.
		void    setScrollTop( int pos );
		void    setAutoScroll( int pos );

		void findText( const QString& text,
		               bool backward,
		               bool caseSensitively,
		               bool highlightSearchResults,
		               std::function<void ( bool found, bool wrapped )> result );

		//! Select the content of the whole page
		void    selectAll();

		//! Copies the selected content to the clipboard
		void    selectedCopy();

		//! Returns the window title
		QString title() const;

		bool    canGoBack() const;

		bool    canGoForward() const;

	public slots:
		void    zoomIncrease();
		void    zoomDecrease();

	protected:
		bool            openPage( const QUrl& url );
		void            handleStartPageAsImage( QUrl& link );

		// Overridden to change the source
		void            setSource( const QUrl& name );
		QUrl            anchorAt( const QPoint& pos );

		// Overloaded to provide custom context menu
		void            contextMenuEvent( QContextMenuEvent* e );
		void            mouseReleaseEvent( QMouseEvent* event );

	private slots:
		void            onLinkClicked( const QUrl& link );
		// Used to restore the scrollbar position and the navigation button status
		void            onLoadFinished( bool ok );

	private:
		QString                 m_lastSearchedWord;

		// Keeps the scrollbar position to move after the page is loaded
		int                     m_storedScrollbarPosition;
};

#endif
