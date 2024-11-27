/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2022  Nick Egorrov, nicegorov@yandex.ru
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

#include <QClipboard>
#include "QGuiApplication"  // for QGuiApplication
#include <QMetaObject>
#include <QString>
#include <QUrl>
#include <Qt>
#include <QtGlobal>

class QPoint;

#include <ubrowser/browser.hpp>
#include <ubrowser/content-provider.hpp>
#include <ubrowser/history-impl.hpp>
#include <ubrowser/history.hpp>
#include <ubrowser/types.hpp>

#include <ubrowser/abstractbrowser.hpp>


UBrowser::AbstractBrowser::AbstractBrowser( ContentProvider::Ptr contentProvider,
                                            QObject* parent ) :
	UBrowser::Browser( contentProvider, parent )
{
	m_history = new UBrowser::HistoryImpl( this );
	connect( m_history,
	         &UBrowser::HistoryImpl::restoreRequest,
	         this,
	         &UBrowser::AbstractBrowser::loadFromHistory );
	m_applyAutoScroll = false;
	m_recoveryIndex = -1;
	m_recoveryMode = false;
	m_requestUrl.clear();
	m_currentUrl.clear();
	m_hoveredUrl.clear();
}

UBrowser::AbstractBrowser::~AbstractBrowser()
{
}

QUrl UBrowser::AbstractBrowser::url() const
{
	return m_currentUrl;
}

void UBrowser::AbstractBrowser::load( const QUrl& address )
{
	if ( !m_history->isEmpty() )
	{
		m_history->currentItem().scroll = scrollTop();
	}

	m_recoveryMode = false;
	m_requestUrl = address;
	loadImpl( address );
}

void UBrowser::AbstractBrowser::reload()
{
	if ( !m_requestUrl.isEmpty() )
	{
		loadImpl( m_requestUrl );
	}
	else if ( !m_history->isEmpty() )
	{
		m_history->currentItem().scroll = scrollTop();
		loadFromHistory( m_history->currentItemIndex() );
	}
}

void UBrowser::AbstractBrowser::loadFromHistory( int index )
{
	m_recoveryIndex = index;
	m_recoveryMode = true;
	m_autoScroll = m_history->item( m_recoveryIndex ).scroll;
	m_applyAutoScroll = true;
	m_requestUrl = m_history->item( index ).url;
	loadImpl( m_requestUrl );
}

void UBrowser::AbstractBrowser::onLoadFinished( bool ok )
{
	if ( ok )
	{
		m_currentUrl = m_requestUrl;
		m_requestUrl.clear();

		if ( m_applyAutoScroll )
		{
			setScrollTop( m_autoScroll );
			m_applyAutoScroll = false;
		}

		if ( m_recoveryMode )
		{
			m_history->restoreResponse( m_recoveryIndex );
		}
		else
		{
			m_history->add( {m_currentUrl, scrollTop(), title()} );
		}
	}

	m_recoveryMode = false;
	emit loadFinished( ok );
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

qreal UBrowser::AbstractBrowser::zoomFactor() const
{
	return 1.0;
}

void UBrowser::AbstractBrowser::setZoomFactor( qreal zoom )
{
	Q_UNUSED( zoom );
}

int UBrowser::AbstractBrowser::scrollTop()
{
	return 0;
}

void UBrowser::AbstractBrowser::setScrollTop( int pos )
{
	Q_UNUSED( pos );
}

void UBrowser::AbstractBrowser::setAutoScroll( int pos )
{
	if ( pos >= 0 )
	{
		m_autoScroll = pos;
		m_applyAutoScroll = true;
	}
}

void UBrowser::AbstractBrowser::selectAll()
{
	;
}

QString UBrowser::AbstractBrowser::selectedText() const
{
	return{};
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

void UBrowser::AbstractBrowser::findText( const QString& text,
                                          bool backward,
                                          bool caseSensitively,
                                          bool highlightSearchResults,
                                          std::function<void ( bool found, bool wrapped )> result )
{
	Q_UNUSED( text );
	Q_UNUSED( backward );
	Q_UNUSED( caseSensitively );
	Q_UNUSED( highlightSearchResults );

	QMetaObject::invokeMethod(
	    this,
	    "onFindTextFinished",
	    Qt::QueuedConnection,
	    Q_ARG( bool, false ) );
	result( false, false );
}

void UBrowser::AbstractBrowser::print( QPrinter* printer, std::function<void ( bool )> result )
{
	Q_UNUSED( printer );

	QMetaObject::invokeMethod(
	    this,
	    "onPrintFinished",
	    Qt::QueuedConnection,
	    Q_ARG( bool, false ) );
	result( false );
}

void UBrowser::AbstractBrowser::onUrlChanged( const QUrl& newUrl )
{
	m_currentUrl = newUrl;
	emit urlChanged( m_currentUrl );
}

void UBrowser::AbstractBrowser::onLinkHovered( const QUrl link )
{
	m_hoveredUrl = link;
}

void UBrowser::AbstractBrowser::onContextMenuRequested( const QPoint& pos )
{
	emit contextMenuRequested( pos, m_hoveredUrl );
}

void UBrowser::AbstractBrowser::onLinkClick( QUrl link )
{
	if ( link.isEmpty() )
	{
		return;
	}

	Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();
	UBrowser::OpenMode mode = UBrowser::OPEN_IN_CURRENT;

	if ( mods & Qt::ShiftModifier )
	{
		mode = UBrowser::OPEN_IN_NEW;
	}
	else if ( mods & Qt::ControlModifier )
	{
		mode = UBrowser::OPEN_IN_BACKGROUND;
	}

	emit linkClicked( link, mode );
}

void UBrowser::AbstractBrowser::onLinkClick()
{
	onLinkClick( m_hoveredUrl );
}

void UBrowser::AbstractBrowser::onFindTextFinished( bool hit, int count, int pos )
{
	emit findTextFinished( hit, count, pos );
}

void UBrowser::AbstractBrowser::onFindTextFinished( bool hit )
{
	onFindTextFinished( hit, -1, -1 );
}

void UBrowser::AbstractBrowser::onPrintFinished( bool ok )
{
	emit printFinished( ok );
}
