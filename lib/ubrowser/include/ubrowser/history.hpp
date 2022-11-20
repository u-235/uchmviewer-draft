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

#ifndef UBROWSER_HISTORY_HPP
#define UBROWSER_HISTORY_HPP

#include <QObject>
#include <QString>
#include <QUrl>

#include "export.hpp"


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief The UBrowser::HistotyItem class holds an element of browser history.
 */
struct HistoryItem
{
	QUrl    url;
	int     scroll;
	QString title;
};


/**
 * @brief The UBrowser::History interface represents the history of the browser
 * navigation.
 */
class UBROWSER_API_DECL History : public QObject
{
		Q_OBJECT

	public:
		explicit History( QObject* parent ) : QObject( parent ) {}

		/**
		 * @brief Adds a new element to the browsing history.
		 *
		 * All items after the current position are removed first. Then a new
		 * element is added immediately after the current position.
		 *
		 * The method emits the historyChanged() signal.
		 *
		 * @param item New element of the navigation history.
		 */
		virtual void add( const HistoryItem& item ) = 0;

		/**
		 * @brief Checks if it is possible to go back in the navigation
		 * history.
		 *
		 * @return @c true if there is a previous item in the navigation
		 * history.
		 */
		virtual bool canGoBack() const = 0;

		/**
		 * @brief Checks if it is possible to go forward in the navigation
		 * history.
		 *
		 * @return @c true if there is a next item in the navigation history.
		 */
		virtual bool canGoForward() const = 0;

		/**
		 * @brief Loads the previous document from the navigation history.
		 *
		 * If there is a desired item in the navigation history, the method
		 * emits the historyChanged() signal.
		 */
		virtual void back() = 0;

		/**
		 * @brief Loads the next document from the navigation history.
		 *
		 * If there is a desired item in the navigation history, the method
		 * emits the historyChanged() signal.
		 */
		virtual void forward() = 0;

		/**
		 * @brief Loads a document from the specified item in the navigation
		 * history.
		 *
		 * If there is a desired item in the navigation history, the method
		 * emits the historyChanged() signal.
		 *
		 * @param index The index of an item in the browser navigation history.
		 */
		virtual void gotoIndex( int index ) = 0;

		/**
		 * @brief Checks if there is a navigation history.
		 *
		 * @return @c true if the navigation history is empty.
		 */
		virtual bool isEmpty() = 0;

		/**
		 * @brief Returns the number of items in the navigation history.
		 *
		 * @return The number of history items.
		 */
		virtual int size() = 0;

		/**
		 * @brief Returns the item of the navigation history by the specified
		 * index.
		 *
		 * If the specified index is outside the bounds of the navigation history
		 * list, the method returns a spurious element, which does not belong to
		 * the navigation history. In other words, instead of an exception for an
		 * invalid index, the application receives a fake one.
		 *
		 * @param index The index of an item in the navigation history.
		 * @return The item of the navigation history or a fake object.
		 */
		virtual HistoryItem item( int index ) = 0;

		/**
		 * @brief Returns the current item index of the navigation history.
		 *
		 * @return The current item index.
		 */
		virtual int currentItemIndex() = 0;

		/**
		 * @brief Returns the current item of the navigation history.
		 *
		 * If the navigation history is empty, the method returns a spurious
		 * element. This is the same behavior as the item() method.
		 *
		 * @return The current item of the navigation history or a fake object.
		 */
		virtual HistoryItem currentItem() = 0;

		virtual void setCurrentItem( const HistoryItem& item ) = 0;

		/**
		 * @brief Clears the navigation history.
		 *
		 * The method emits the historyChanged() signal.
		 */
		virtual void clear() = 0;

		/**
		 * @brief Returns the navigation history list.
		 *
		 * @return The navigation history list.
		 */
		virtual QList<HistoryItem> items() const = 0;

		/**
		 * @brief Overwrites the navigation history.
		 *
		 * The method emits the historyChanged() signal.
		 *
		 * @param items The list with the navigation history to be copied.
		 * @param index The new index of the current item in the navigation
		 * history.
		 * @param autoLoad
		 */
		virtual void setItems( const QList<HistoryItem>& items, int index = 0, bool autoLoad = true ) = 0;

	signals:
		/**
		 * @brief The signal indicates that the navigation history has changed.
		 *
		 * @see add()
		 * @see back()
		 * @see clear()
		 * @see forward()
		 * @see gotoIndex()
		 * @see setItems()
		 */
		void historyChanged();
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_HISTORY_HPP
