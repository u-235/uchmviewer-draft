/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2021-2025 Nick Egorrov, nicegorov@yandex.ru
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

#include <QApplication>
#include <QUrl>
#include <QWebEngineProfile>
#include <Qt>
#include <QtGlobal>

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>

#include "qtwebengineurlschemehandler.h"

#include "qtwebenginepage.h"


#ifdef PRINT_DEBUG
	#undef PRINT_DEBUG
#endif

#if defined PRINT_DEBUG_ALL || defined PRINT_DEBUG_UBROWSER_ALL || defined PRINT_DEBUG_QTWEBENGINE
	#include <QDebug>
	#define PRINT_DEBUG
#endif


QtWebEngine::Page::Page( UBrowser::ContentProvider::Ptr content, QObject* parent )
	: QWebEnginePage( parent )
{
	QWebEngineProfile* pf = profile();
	pf->removeAllUrlSchemeHandlers();
	pf->installUrlSchemeHandler( EBOOK_URL_SCHEME, new QtWebEngine::UrlSchemeHandler( content, this ) );
	connect( this, &QtWebEngine::Page::linkHovered, this, &QtWebEngine::Page::onLinkHovered );
}

QtWebEngine::Page::~Page()
{
}

/* Link click capture. This does not work for the right mouse button.
 * Important! In Qt5, this function is called both when clicking with
 * the Ctrl and/or Shift keys pressed and when clicking without keyboard
 * modifiers, whereas in Qt6 it is called only when clicking without
 * keyboard modifiers. In any Qt, the createWindow function is called
 * when clicking with keyboard modifier.
 */
bool QtWebEngine::Page::acceptNavigationRequest( const QUrl& url, NavigationType type, bool isMainFrame )
{
#ifdef PRINT_DEBUG
	qDebug() << "QtWebEngine.Page.acceptNavigationRequest";
	qDebug() << "  url = " << url.toString();
	qDebug() << "  type  = " << type;
	qDebug() << "  isMainFrame  = " << isMainFrame;
#else
	Q_UNUSED( isMainFrame );
#endif

	if ( type == QWebEnginePage::NavigationTypeLinkClicked )
	{
		Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
		bool enableEmit = ( mods & ( Qt::ShiftModifier | Qt::ControlModifier ) ) == 0;

		/*
		 * Emitting the linkClicked signal directly from here worked until Qt 6.10.
		 * In 6.10, this sometimes caused the application to crash. The solution was
		 * to use a timer to emit the signal asynchronously. Another way was to
		 * connect the WebEnginePage::linkClicked signal to the ViewWindow::onLinkClicked
		 * slot via a queue.
		 */
		if ( enableEmit )
			emit linkClicked( url, UBrowser::OpenMode::open_in_current );

		return false;
	}

	return true;
}

/*
 * If the link has the "target = _new" attribute, then the WebEngine tries to create
 * a new page using the createWindow function. However, the old page does not receive
 * an acceptNavigationRequest unless the Ctrl or Shift key has been pressed.
 *
 * The createWindow function is also used from JavaScript to create a new page, but this
 * does not work in this implementation.
 */
QWebEnginePage* QtWebEngine::Page::createWindow( WebWindowType type )
{
#ifdef PRINT_DEBUG
	qDebug() << "QtWebEngine.Page.createWindow";
	qDebug() << "  type = " << type;
#else
	Q_UNUSED( type );
#endif

	if ( !m_url.isEmpty() )
	{
		Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();

		if ( ( mods & Qt::ShiftModifier ) != 0 || type == QWebEnginePage::WebBrowserTab )
			emit linkClicked( m_url, UBrowser::OpenMode::open_in_new );
		else
			emit linkClicked( m_url, UBrowser::OpenMode::open_in_background );
	}

	return nullptr;
}

void QtWebEngine::Page::onLinkHovered( const QString& url )
{
#ifdef PRINT_DEBUG
	qDebug() << "QtWebEngine.Page.linkHovered";
	qDebug() << "  url = " << url;
#endif

	m_url = url;
}
