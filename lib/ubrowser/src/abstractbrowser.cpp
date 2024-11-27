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
#include <QDebug>
#include <QString>
#include <QUrl>
#include <QTimer>
#include <QtGlobal>
#include <QWidget>

class QPoint;

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/history-impl.hpp>
#include <ubrowser/history.hpp>
#include <ubrowser/types.hpp>

#include <ubrowser/abstractbrowser.hpp> // IWYU pragma: associated


#ifndef ABSTRACT_BROWSER_DEFAULT_REVEAL_DELAY
	#define ABSTRACT_BROWSER_DEFAULT_REVEAL_DELAY 30
#endif


UBrowser::AbstractBrowser::AbstractBrowser( ContentProvider::Ptr contentProvider,
                                            QObject* parent ) :
	UBrowser::Browser( contentProvider, parent ),
	m_fireRevealOnLoad{false},
	m_fireRevealIfShow{false},
	m_fireRevealDelay{ABSTRACT_BROWSER_DEFAULT_REVEAL_DELAY},
	m_pageLoads{false},
	m_pageReveal{false},
	m_untimelyPageReveal{false},
	m_untimelyUrlChanged{false},
	m_history{new HistoryImpl( this )},
	m_requestUrl{},
	m_currentUrl{},
	m_changedUrl{},
	m_fragmentScrollInvalid{false},
	m_fragmentScroll{-1},
	m_scrollTop{0}
{
	connect( m_history, &History::historyChanged,
	         this, &AbstractBrowser::onHistoryChanged );
}

UBrowser::AbstractBrowser::~AbstractBrowser()
{
}

QUrl UBrowser::AbstractBrowser::url() const
{
	return m_currentUrl;
}

void UBrowser::AbstractBrowser::setUrl( const QUrl& url )
{
	m_requestUrl.clear();

	if ( m_currentUrl != url )
	{
		m_currentUrl = url;
		emit urlChanged( m_currentUrl );
	}
}

void UBrowser::AbstractBrowser::load( const QUrl& ebookUrl )
{
	m_requestUrl = ebookUrl;
	loadConverted( m_requestUrl );
}

void UBrowser::AbstractBrowser::reload()
{
	if ( !m_requestUrl.isEmpty() )
	{
		loadConverted( m_requestUrl );
	}
	else if ( !m_currentUrl.isEmpty() )
	{
		setAutoScroll( scrollTop() );
		loadConverted( m_currentUrl );
	}
}

void UBrowser::AbstractBrowser::loadConverted( const QUrl& ebookUrl )
{
	loadImpl( contentProvider()->convertUrlForBrowser( ebookUrl ) );
}

void UBrowser::AbstractBrowser::injectJS()
{
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

int UBrowser::AbstractBrowser::scrollTop()
{
	int a = autoScroll();
	return a >= 0 ? a : m_scrollTop;
}

void UBrowser::AbstractBrowser::rePageReveal()
{
	onPageReveal();
}

void UBrowser::AbstractBrowser::reUrlChanged()
{
	onUrlChanged( m_changedUrl );
	m_changedUrl.clear();
}

void UBrowser::AbstractBrowser::onLoadStarted()
{
	qDebug() << "[DEBUG] AbstractBrowser::onLoadStarted";
	m_pageLoads = true;
	m_pageReveal = false;
	emit loadStarted();
}

void UBrowser::AbstractBrowser::onLoadFinished( bool successfully )
{
	qDebug() << "[DEBUG] AbstractBrowser::onLoadFinished";
	qDebug() << "    status =" << successfully;
	m_pageLoads = false;
	m_pageReveal = !successfully;

	if ( successfully )
	{
		injectJS();

		if ( m_untimelyUrlChanged )
			reUrlChanged();

		if ( m_untimelyPageReveal )
			rePageReveal();
		else if ( m_fireRevealOnLoad || ( m_fireRevealIfShow && view()->isVisible() ) )
			QTimer::singleShot( m_fireRevealDelay, this, &AbstractBrowser::onPageReveal );
	}

	m_untimelyUrlChanged = false;
	m_untimelyPageReveal = false;
	m_fragmentScrollInvalid = successfully;
	emit loadFinished( successfully );
}

void UBrowser::AbstractBrowser::onPageReveal()
{
	qDebug() << "[DEBUG] AbstractBrowser::onPageReveal";
	m_untimelyPageReveal = m_pageLoads;

	if ( m_pageLoads )
	{
		qDebug() << "    The page is loading. Do nothing.";
		return;
	}

	if ( m_pageReveal )
	{
		qDebug() << "    The page already reveal. Do nothing.";
		return;
	}

	qDebug() << "    OK. Handle signal.";
	m_pageReveal = true;
	scrollAfterLoad();
}

void UBrowser::AbstractBrowser::onUrlChanged( const QUrl& url )
{
	qDebug() << "[DEBUG] AbstractBrowser::onUrlChanged";
	qDebug() << "    url =" << url.toString();
	m_untimelyUrlChanged = m_pageLoads;

	if ( m_pageLoads )
	{
		qDebug() << "    The page is loading. Do nothing.";
		m_changedUrl = url;
		return;
	}

	qDebug() << "    OK. Handle signal.";
	m_fragmentScrollInvalid = true;
	setUrl( contentProvider()->convertUrlForEbook( url ) );
}

void UBrowser::AbstractBrowser::onHistoryChanged()
{
	qDebug() << "[DEBUG] AbstractBrowser::onHistoryChanged";

	emit historyChanged();
}

void UBrowser::AbstractBrowser::onScrollChanged( int scroll )
{
	qDebug() << "[DEBUG] AbstractBrowser::onScrollChanged";
	qDebug() << "    scroll =" << scroll;

	m_scrollTop = scroll;

	if ( m_fragmentScrollInvalid )
	{
		m_fragmentScroll = url().hasFragment() ? scroll : -1;
		m_fragmentScrollInvalid = false;
	}

	m_history->onScrollChanged( scroll == m_fragmentScroll ? -1 : scroll );
}

void UBrowser::AbstractBrowser::onLinkClicked( const QUrl& link, OpenMode mode )
{
	qDebug() << "[DEBUG] AbstractBrowser::onLinkClicked";
	qDebug() << "    link =" << link.toString();

	emit linkClicked( contentProvider()->convertUrlForEbook( link ), mode );
}

void UBrowser::AbstractBrowser::onContextMenuRequested( const QPoint& globalPos, const QUrl& link )
{
	qDebug() << "[DEBUG] AbstractBrowser::onContextMenuRequested";

	emit contextMenuRequested( globalPos, contentProvider()->convertUrlForEbook( link ) );
}

void UBrowser::AbstractBrowser::onFindTextFinished( bool hit, int count, int pos )
{
	qDebug() << "[DEBUG] AbstractBrowser::onFindTextFinished";

	emit findTextFinished( hit, count, pos );
}

void UBrowser::AbstractBrowser::onFindTextFinished( bool hit )
{
	onFindTextFinished( hit, -1, -1 );
}

void UBrowser::AbstractBrowser::onPrintFinished( bool ok )
{
	qDebug() << "[DEBUG] AbstractBrowser::onPrintFinished" << ok;

	emit printFinished( ok );
}
