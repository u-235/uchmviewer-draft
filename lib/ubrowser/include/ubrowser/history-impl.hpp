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

#ifndef UBROWSER_HISTORY_IMPL_HPP
#define UBROWSER_HISTORY_IMPL_HPP

#include <QList>
#include <QObject>
#include <QString>

#include <ubrowser/history.hpp>

#include "export.hpp"


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief The UBrowser::HistoryImpl class implements the UBrowser::History
 * interface.
 *
 * To control how pages are loaded from the history URL, the UBrowser::HistoryImpl
 * class has a restoreRequest() signal and a restoreResponse() slot. If the
 * history contains the desired item, calling the back(), forward() or
 * gotoIndex() methods sends the restoreRequest() signal with the history item
 * index. In response, the UBrowser::Controll class must try to load the page at
 * the URL from the specified history element and, if successful, call
 * restoreResponse() with the same index.
 */
class UBROWSER_API_DECL HistoryImpl : public UBrowser::History
{
		Q_OBJECT

	public:
		HistoryImpl( QObject* parent );

		~HistoryImpl();

		void add( UBrowser::HistoryItem item ) override;

		bool canGoBack() const override;

		bool canGoForward() const override;

		/**
		 * @copydoc UBrowser::History::back()
		 * @note If the history contains the desired item, this method triggers
		 * the restoreRequest() signal.
		 */
		void back() override;

		/**
		 * @copydoc UBrowser::History::forward()
		 * @note If the history contains the desired item, this method triggers
		 * the restoreRequest() signal.
		 */
		void forward() override;

		/**
		 * @copydoc UBrowser::History::gotoIndex()
		 * @note If the history contains the desired item, this method triggers
		 * the restoreRequest() signal.
		 */
		void gotoIndex( int index ) override;

		virtual bool isEmpty() override;

		int size() override;

		/**
		 * @copydoc UBrowser::History::item()
		 *
		 * If @p index falls outside the history item list, the method returns
		 * a fake UBrowser::HistoryItem object. See #m_fakeItem
		 */
		UBrowser::HistoryItem& item( int index ) override;

		int currentItemIndex() override;

		/**
		 * @copydoc UBrowser::History::currentItem()
		 *
		 * If the history is empty, the method returns a fake UBrowser::HistoryItem
		 * object. See #m_fakeItem
		 */
		UBrowser::HistoryItem& currentItem() override;

		void clear() override;

		const QList<UBrowser::HistoryItem>& items() const override;

		void setItems( const QList<UBrowser::HistoryItem>& items, int index = 0 ) override;

	public slots:
		/**
		 * @brief Response on request to load from history.
		 *
		 * @param index The new index of the current history item.
		 * @see restoreRequest()
		 */
		void restoreResponse( int index );

	signals:
		/**
		 * @brief Requests to load the page by URL from the history.
		 *
		 * This signal is emitted when back(), forward() and gotoIndex() methods
		 * are activated.
		 *
		 * @param index Index of the history item to restore the page.
		 * @see restoreResponse()
		 */
		void restoreRequest( int index );

	protected:
		int m_currentItemIndex;
		QList<UBrowser::HistoryItem> m_items;
		/// History item returned for an invalid index.
		UBrowser::HistoryItem m_fakeItem;
};

/// @}
// End BrowserAPI implementation group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_HISTORY_IMPL_HPP
