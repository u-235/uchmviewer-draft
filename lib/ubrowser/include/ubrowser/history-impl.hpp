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

#ifndef UBROWSER_HISTORY_IMPL_HPP
#define UBROWSER_HISTORY_IMPL_HPP

#include <QList>
#include <QObject>
#include <QString>

class QUrl;

#include <ubrowser/history.hpp>

#include "export.hpp"


namespace UBrowser
{

class Browser;

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief The UBrowser::HistoryImpl class implements the UBrowser::History
 * interface.
 */
class UBROWSER_API_DECL HistoryImpl : public UBrowser::History
{
		Q_OBJECT

	public:
		HistoryImpl( UBrowser::Browser* parent );

		~HistoryImpl() override;

		void add( const UBrowser::HistoryItem& item ) override;
		bool canGoBack() const override;
		bool canGoForward() const override;
		void back() override;
		void forward() override;
		void gotoIndex( int index ) override;
		bool isEmpty() override;
		int size() override;
		UBrowser::HistoryItem item( int index ) override;
		int currentItemIndex() override;
		UBrowser::HistoryItem currentItem() override;
		void setCurrentItem( const HistoryItem& item ) override;
		void clear() override;
		QList<UBrowser::HistoryItem> items() const override;
		void setItems( const QList<UBrowser::HistoryItem>& items, int index = 0, bool autoLoad = true ) override;

	public slots:
		void onLoadFinished( bool ok );
		void onUrlChanged( const QUrl& url );
		void onScrollChanged( int scroll );

	protected:
		bool addEmpty();
		void restore( int index );

		Browser* m_browser;
		int m_currentItemIndex;
		int m_restoredItemIndex;
		QList<UBrowser::HistoryItem> m_items;
};

/// @}
// End BrowserAPI implementation group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_HISTORY_IMPL_HPP
