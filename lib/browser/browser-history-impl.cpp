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

#include <QString>
#include <QUrl>

#include <browser/browser-history-impl.hpp>


BrowserHistoryImpl::BrowserHistoryImpl(QObject* parent) :
	BrowserHistory(parent)
{
	m_currentItemIndex = 0;
}

BrowserHistoryImpl::~BrowserHistoryImpl()
{
}

void BrowserHistoryImpl::add(BrowserHistoryItem item)
{
	m_currentItemIndex++;
	// If we are not at the end of the story, we remove the tail
	// from the current position before adding a new item.
	while (m_items.size() > m_currentItemIndex) {
		m_items.removeLast();
	}

	m_items.append(item);
	emit historyChanged();
}

bool BrowserHistoryImpl::canGoBack() const
{
	return m_currentItemIndex > 0;
}

bool BrowserHistoryImpl::canGoForward() const
{
	return m_currentItemIndex < m_items.count() - 1;
}

void BrowserHistoryImpl::back()
{
	gotoIndex(--m_currentItemIndex);
}

void BrowserHistoryImpl::forward()
{
	gotoIndex(++m_currentItemIndex);
}

void BrowserHistoryImpl::gotoIndex(int index)
{
	if (index < 0 || index > m_items.count() - 1) {
		return;
	}

	emit restoreRequest(index);
}

bool BrowserHistoryImpl::isEmpty()
{
	return m_items.count() == 0;
}

int BrowserHistoryImpl::size()
{
	return m_items.count();
}

BrowserHistoryItem& BrowserHistoryImpl::item(int index)
{
	if (index < 0 || index > m_items.count() - 1) {
		m_fakeItem.url.clear();
		m_fakeItem.scroll = -1;
		m_fakeItem.title.clear();
		return m_fakeItem;
	}

	return m_items[index];
}

int BrowserHistoryImpl::currentItemIndex()
{
	return m_currentItemIndex;
}

BrowserHistoryItem& BrowserHistoryImpl::currentItem()
{
	return item(m_currentItemIndex);
}

void BrowserHistoryImpl::clear()
{
	m_currentItemIndex = 0;
	m_items.clear();
	emit historyChanged();
}

const QList<BrowserHistoryItem>& BrowserHistoryImpl::items() const
{
	return m_items;
}

void BrowserHistoryImpl::setItems(const QList<BrowserHistoryItem>& items, int index)
{
	m_items = items;
	int i = index < m_items.size() ? index : m_items.size() - 1;
	i = i < 0 ? 0 : i;
	m_currentItemIndex = i;
	emit historyChanged();
}

void BrowserHistoryImpl::restoreResponse(int index)
{
	m_currentItemIndex = index;
	emit historyChanged();
}
