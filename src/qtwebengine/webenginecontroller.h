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

#include <functional>   // for function

#include <QObject>          // for slots, Q_OBJECT, signals
#include <QString>          // for QString
#include <QUrl>             // for QUrl
#include <QWebEngineView>   // for QWebEngineView
#include <QtGlobal>         // for qreal

class QContextMenuEvent;
class QMenu;
class QPoint;
class QPrinter;
class QWidget;

#include <browser/controller.hpp>
#include <browser/types.hpp>    // for OPEN_IN_CURRENT, OpenMode

namespace Browser {
struct Settings;
}

class WebEngineWidget;


class WebEngineController : public Browser::Controller
{
		Q_OBJECT

	public:
		WebEngineController( QWidget* parent );
		virtual ~WebEngineController();

		QWidget* view() override;

		bool hasOption(Browser::Option option) override;

		//! Open a page from current chm archive
		void load (const QUrl& url ) override;

		QUrl url() const override;

	public:
		// Apply the configuration settings (JS enabled etc) to the web renderer
		static  void    applySettings(Browser::Settings& settings);

		//! Invalidate current view, doing all the cleanups etc.
		void    invalidate() override;

		//! Prints the current page on the printer.
		void print( QPrinter* printer, std::function<void (bool success)> result ) override;

		//! Return current ZoomFactor.
		qreal   zoomFactor() const override;

		//! Sets ZoomFactor. The value returned by getZoomFactor(), given to this function, should give the same result.
		void    setZoomFactor( qreal zoom ) override;

		/*!
		* Return current scrollbar position in view window. Saved on program exit.
		* There is no restriction on returned value, except that giving this value to
		* setScrollbarPosition() should move the scrollbar in the same position.
		*/
		int     scrollTop() override;

		//! Sets the scrollbar position.
		void    setScrollTop(int pos) override;
		void    setAutoScroll(int pos) override;

		void findText(const QString& text,
		              bool backward,
		              bool caseSensitively,
		              bool highlightSearchResults,
		              std::function<void (bool found, bool wrapped)> result) override;

		//! Select the content of the whole page
		void    selectAll() override;

		//! Copies the selected content to the clipboard
		void    selectedCopy() override;

		//! Returns the window title
		QString title() const override;

		bool    canGoBack() const override;

		bool    canGoForward() const override;

		void back() override;

		void forward() override;

	protected:
		bool            openPage ( const QUrl& url );
		void            handleStartPageAsImage( QUrl& link );

		// Overriden to change the source
		void            setSource ( const QUrl& name );

	private slots:
		// Used to restore the scrollbar position and the navigation button status
		void            onLoadFinished ( bool success );
		void            onLinkClicked(const QUrl& url, Browser::OpenMode mode = Browser::OPEN_IN_CURRENT);

	private:
		WebEngineWidget* m_widget;
		QMenu*                  m_contextMenu;

		// Keeps the scrollbar position to move after the page is loaded
		int                     m_storedScrollbarPosition;
};

#endif // QTWEBENGINE_VIEWWINDOW_H
