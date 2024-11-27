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

#include <QGuiApplication>
#include <QClipboard>
#include <QString>
#include <QUrl>
#include <QtGlobal>

class QPoint;

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/history-impl.hpp>
#include <ubrowser/history.hpp>

#include <ubrowser/abstractbrowser.hpp> // IWYU pragma: associated


#ifdef PRINT_DEBUG
	#undef PRINT_DEBUG
#endif

#if defined PRINT_DEBUG_ALL || defined PRINT_DEBUG_UBROWSER
	#include <QDebug>
	#define PRINT_DEBUG
#endif


UBrowser::AbstractBrowser::AbstractBrowser( ContentProvider::Ptr contentProvider, QObject* parent ) :
	UBrowser::Browser( contentProvider, parent ),
	m_pageLoads{true},
	m_pageReveal{false},
	m_prematurePageReveal{false},
	m_prematureUrlChanged{false},
	m_history{new HistoryImpl( this )},
	m_requestUrl{},
	m_currentUrl{},
	m_changedUrl{}
{
	connect( m_history, &History::historyChanged, this, &AbstractBrowser::onHistoryChanged );
	connect( this, &Browser::urlChanged, m_history, &HistoryImpl::onUrlChanged );
}

UBrowser::AbstractBrowser::~AbstractBrowser()
{
}

QUrl UBrowser::AbstractBrowser::url() const
{
	return m_currentUrl;
}

void UBrowser::AbstractBrowser::load( const QUrl& url )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.load";
#endif

	m_requestUrl = url;
	loadPage( m_requestUrl );
}

void UBrowser::AbstractBrowser::reload()
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.reload";
#endif

	if ( !m_requestUrl.isEmpty() )
	{
		loadPage( m_requestUrl );
	}
	else if ( !m_currentUrl.isEmpty() )
	{
		setAutoScroll( scrollTop() );
		loadPage( m_currentUrl );
	}
}

void UBrowser::AbstractBrowser::injectJS()
{
}

void UBrowser::AbstractBrowser::saveScrollInHistory()
{
	m_history->setScroll( scrollTop() );
}

UBrowser::History* UBrowser::AbstractBrowser::history() const
{
	return m_history;
}

bool UBrowser::AbstractBrowser::canGoBack() const
{
	return m_history->canGoBack();
}

bool UBrowser::AbstractBrowser::canGoForward() const
{
	return m_history->canGoForward();
}

void UBrowser::AbstractBrowser::back()
{
	m_history->back();
}

void UBrowser::AbstractBrowser::forward()
{
	m_history->forward();
}

void UBrowser::AbstractBrowser::selectedCopy()
{
	QString text = selectedText();

	if ( !text.isEmpty() )
	{
		QClipboard* clip = QGuiApplication::clipboard();
		clip->setText( text );
	}
}

void UBrowser::AbstractBrowser::onLoadStarted()
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onLoadStarted";
#endif

	m_pageLoads = true;
	m_pageReveal = false;
	emit loadStarted();
}

void UBrowser::AbstractBrowser::onLoadFinished( bool successfully )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onLoadFinished";
	qDebug() << "    status =" << successfully;
#endif

	m_pageLoads = false;
	m_pageReveal = !successfully;

	if ( successfully )
	{
		m_requestUrl.clear();
		injectJS();

		if ( m_prematureUrlChanged )
			onUrlChanged( m_changedUrl );

		if ( m_prematurePageReveal )
			onPageReveal();
	}

	m_prematureUrlChanged = false;
	m_prematurePageReveal = false;
	emit loadFinished( successfully );
}

void UBrowser::AbstractBrowser::onPageReveal()
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onPageReveal";
#endif

	m_prematurePageReveal = m_pageLoads;

	if ( m_pageLoads )
	{
#ifdef PRINT_DEBUG
		qDebug() << "    The page is loading. Do nothing.";
#endif

		return;
	}

	if ( m_pageReveal )
	{
#ifdef PRINT_DEBUG
		qDebug() << "    The page already reveal. Do nothing.";
#endif

		return;
	}

#ifdef PRINT_DEBUG
	qDebug() << "    OK. Handle signal.";
#endif

	m_pageReveal = true;
	m_history->setScrollFragment( m_currentUrl.hasFragment() ? scrollTop() : 0 );
	scrollAfterLoad();
}

void UBrowser::AbstractBrowser::onUrlChanged( const QUrl& url )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onUrlChanged";
	qDebug() << "    url =" << url.toString();
#endif

	m_prematureUrlChanged = m_pageLoads;

	if ( m_pageLoads )
	{
#ifdef PRINT_DEBUG
		qDebug() << "    The page is loading. Do nothing.";
#endif

		m_changedUrl = url;
		return;
	}

	if ( m_currentUrl == url )
	{
#ifdef PRINT_DEBUG
		qDebug() << "    The url same as the current. Do nothing.";
#endif
		m_changedUrl = url;
		return;
	}

#ifdef PRINT_DEBUG
	qDebug() << "    OK. Handle signal.";
#endif

	m_currentUrl = url;
	m_history->setScrollFragment( m_currentUrl.hasFragment() ? scrollTop() : 0 );
	emit urlChanged( m_currentUrl );
}

void UBrowser::AbstractBrowser::onHistoryChanged()
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onHistoryChanged";
#endif

	emit historyChanged();
}

void UBrowser::AbstractBrowser::onScrollChanged( int scroll )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onScrollChanged";
	qDebug() << "    scroll =" << scroll;
#else
	Q_UNUSED( scroll );
#endif

	m_history->setScroll( scroll );
}

void UBrowser::AbstractBrowser::onLinkClicked( const QUrl& link, OpenMode mode )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onLinkClicked";
	qDebug() << "    link =" << link.toString();
#endif

	m_history->setScroll( scrollTop() );
	emit linkClicked( link, mode );
}

void UBrowser::AbstractBrowser::onContextMenuRequested( const QPoint& globalPos, const QUrl& link )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onContextMenuRequested";
#endif

	emit contextMenuRequested( globalPos, link );
}

void UBrowser::AbstractBrowser::onFindTextFinished( bool hit, int count, int pos )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onFindTextFinished";
#endif

	emit findTextFinished( hit, count, pos );
}

void UBrowser::AbstractBrowser::onFindTextFinished( bool hit )
{
	onFindTextFinished( hit, -1, -1 );
}

void UBrowser::AbstractBrowser::onPrintFinished( bool ok )
{
#ifdef PRINT_DEBUG
	qDebug() << "AbstractBrowser.onPrintFinished";
	qDebug() << "    status : " << ok;
#endif

	emit printFinished( ok );
}
