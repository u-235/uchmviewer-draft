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

#include <QString>
#include <QUrl>

#include <ubrowser/browser.hpp>
#include <ubrowser/history.hpp>

#include <ubrowser/history-impl.hpp>    // IWYU pragma: associated


UBrowser::HistoryImpl::HistoryImpl( UBrowser::Browser* parent ) :
	UBrowser::History( parent ),
	m_browser{parent},
	m_currentItemIndex{0},
	m_restoredItemIndex{-1},
	m_items{HistoryItem{}}
{
	connect( m_browser, &Browser::loadFinished,
	         this, &HistoryImpl::onLoadFinished );
	connect( m_browser, &Browser::urlChanged,
	         this, &HistoryImpl::onUrlChanged );
}

UBrowser::HistoryImpl::~HistoryImpl()
{
}

void UBrowser::HistoryImpl::add( const HistoryItem& item )
{
	addEmpty();
	setCurrentItem( item );
	emit historyChanged();
}

bool UBrowser::HistoryImpl::canGoBack() const
{
	return m_currentItemIndex > 0;
}

bool UBrowser::HistoryImpl::canGoForward() const
{
	return m_currentItemIndex < m_items.size() - 1;
}

void UBrowser::HistoryImpl::back()
{
	gotoIndex( m_currentItemIndex - 1 );
}

void UBrowser::HistoryImpl::forward()
{
	gotoIndex( m_currentItemIndex + 1 );
}

void UBrowser::HistoryImpl::gotoIndex( int index )
{
	if ( index < 0 || index > m_items.size() - 1 )
	{
		return;
	}

	restore( index );
}

bool UBrowser::HistoryImpl::isEmpty()
{
	return m_items.size() > 1;
}

int UBrowser::HistoryImpl::size()
{
	return m_items.size();
}

UBrowser::HistoryItem UBrowser::HistoryImpl::item( int index )
{
	if ( index < 0 || index > m_items.size() - 1 )
		return HistoryItem{};

	return m_items[index];
}

int UBrowser::HistoryImpl::currentItemIndex()
{
	return m_currentItemIndex;
}

UBrowser::HistoryItem UBrowser::HistoryImpl::currentItem()
{
	return item( m_currentItemIndex );
}

void UBrowser::HistoryImpl::setCurrentItem( const HistoryItem& item )
{
	m_items[m_currentItemIndex] = item;
}

void UBrowser::HistoryImpl::clear()
{
	const HistoryItem& curr = currentItem();
	m_items.clear();
	m_currentItemIndex = 0;
	m_items.append( curr );
	emit historyChanged();
}

QList<UBrowser::HistoryItem> UBrowser::HistoryImpl::items() const
{
	return m_items;
}

void UBrowser::HistoryImpl::setItems( const QList<UBrowser::HistoryItem>& items, int index, bool autoLoad )
{
	m_items = items;
	int i = index < m_items.size() ? index : m_items.size() - 1;
	i = i < 0 ? 0 : i;

	if ( autoLoad )
	{
		restore( i );
	}
	else
	{
		m_currentItemIndex = i;
		emit historyChanged();
	}
}

void UBrowser::HistoryImpl::onLoadFinished( bool ok )
{
	if ( !ok )
		m_restoredItemIndex = -1;
}

void UBrowser::HistoryImpl::onUrlChanged( const QUrl& url )
{
	if ( m_restoredItemIndex >= 0 )
	{
		m_currentItemIndex = m_restoredItemIndex;
		m_restoredItemIndex = -1;
	}

	if ( !currentItem().url.matches( url,  QUrl::None ) )
	{
		if ( !currentItem().url.isEmpty() )
			addEmpty();

		m_items[m_currentItemIndex].url = url;
		m_items[m_currentItemIndex].scroll = 0;
		m_items[m_currentItemIndex].title = m_browser->title();
	}

	emit historyChanged();
}

void UBrowser::HistoryImpl::onScrollChanged( int scroll )
{
	m_items[m_currentItemIndex].scroll = scroll;
}

bool UBrowser::HistoryImpl::addEmpty()
{
	m_currentItemIndex++;

	// If we are not at the end of the story, we remove the tail
	// from the current position before adding a new item.
	while ( m_items.size() > m_currentItemIndex )
		m_items.removeLast();

	m_items.append( HistoryItem{} );
	return true;
}

void UBrowser::HistoryImpl::restore( int index )
{
	m_restoredItemIndex = index;
	const HistoryItem& it = item( index );

	m_browser->setAutoScroll( it.scroll );
	m_browser->load( it.url );
}
