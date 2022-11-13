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

#ifndef CONTENTPROVIDER_HPP
#define CONTENTPROVIDER_HPP

#include <memory>   // std::shared_ptr

class QByteArray;
class QString;
class QUrl;


//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief The Content structure holds a data retrieved from an eBook.
 *
 * See ContentProvider::getContent() method.
 */
struct ContentData
{
	QByteArray  buffer;     ///< Raw data
	QString     mime;       ///< Mime type of data
	QString     encoding;   ///< Current encoding of eBook
};


/**
 * @brief The ContentProvider interface provides access to the contents
 * of an eBook.
 *
 * @details The ContentProvider class performs access to an content by URL.
 * The isSupportedUrl() method allows you to check the validity of an URL.
 * If the URL is supported, the content can be retrieved using the getContent()
 * method.
 *
 * @see ContentData
 * @see ContentProviderPtr
 */
class ContentProvider
{
	public:
		/**
		 * @brief Returns the URL scheme used by an eBook implementation.
		 *
		 * @return A non-empty string with a URL scheme.
		 * @see EBook::urlScheme()
		 */
		virtual QString urlScheme() = 0;

		/**
		 * @brief Checks if this kind of URL is supported by the given eBook
		 * type.
		 *
		 * @details The scheme of the tested URL is compared with the scheme
		 * inherent in the eBook.
		 *
		 * @param[in] url The URL to check.
		 * @return Returns @c true if this kind of URL is supported.
		 * @see EBook::isSupportedUrl()
		 */
		virtual bool isSupportedUrl(const QUrl& url) = 0;

		/**
		 * @brief Fills the ContentData structure with eBook content by URL.
		 *
		 * @details The method tries to retrieve the requested content from
		 * the eBook. If the content exists, the ContentData::mime and
		 * ContentData::encoding fields are also filled. Note that the
		 * ContentData::encoding field makes sense for the MIME type "text/*".
		 * See MimeHelper.
		 *
		 * @param[out] data A structure where the retreived content should be
		 *     stored.
		 * @param[in]  url  An URL in eBook to retreive content from.
		 * @return Returns @c true if operation is succes.
		 */
		virtual bool getContent(ContentData& data, const QUrl& url) const = 0;
};

/**
 * @brief The ContentProviderPtr is a shared pointer to ContentProvider.
 */
typedef std::shared_ptr<ContentProvider> ContentProviderPtr;

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

#endif // CONTENTPROVIDER_HPP
