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

#ifndef UBROWSER_CONTENTPROVIDER_HPP
#define UBROWSER_CONTENTPROVIDER_HPP

#include <memory>

#include <QByteArray>
#include <QString>
#include <QUrl>

#define EBOOK_URL_SCHEME "ebook"


namespace UBrowser
{

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
	QByteArray  mime;       ///< Mime type of data
	QByteArray  asUtf8;     ///< Raw data as text in utf-8
};


/**
 * @brief The UBrowser::ContentProvider interface provides access to the contents
 * of an eBook.
 *
 * @details The UBrowser::ContentProvider class performs access to an content by
 * URL. The isValidUrl() method allows you to check the validity of an URL.
 * If the URL is supported, the content can be retrieved using the content()
 * method.
 *
 * @see ContentData
 * @see ContentProvider::Ptr
 */
class ContentProvider
{
	public:
		/**
		* @brief The UBrowser::ContentProvider::Ptr is a shared pointer to
		* UBrowser::ContentProvider.
		*/
		typedef std::shared_ptr<ContentProvider> Ptr;

		virtual ~ContentProvider() {}

		// Converts the string to the ebook-specific URL format
		virtual QUrl pathToUrl( const QString& link ) const = 0;

		// Extracts the path component from the URL
		virtual QString urlToPath( const QUrl& link ) const = 0;

		/**
		 * @brief Convert an Url ForEbook
		 *
		 * @param browserUrl
		 * @return
		 */
		virtual QUrl convertUrlForEbook( const QUrl& browserUrl ) const = 0;

		virtual QUrl convertUrlForBrowser( const QUrl& ebookUrl ) const = 0;

		/**
		 * @brief Checks if this kind of URL is supported by the given eBook
		 * type.
		 *
		 * The scheme of the tested URL is compared with the scheme
		 * inherent in the eBook.
		 *
		 * @param[in] browserUrl The URL to check.
		 * @return Returns @c true if this kind of URL is supported.
		 * @see EBook::isSupportedUrl()
		 */
		virtual bool isValidUrl( const QUrl& browserUrl ) const = 0;

		/**
		 * @brief Fills the UBrowser::ContentData structure with eBook content
		 * by URL.
		 *
		 * If the required content exists and its MIME type starts with "text",
		 * the UBrowser::ContentData::asUtf8 field is also filled in. Note that
		 * the text in this field always has utf-8 encoding.
		 *
		 * @param[in,out] data A structure where the retrieved content should be
		 *     stored.
		 * @param[in]  browserUrl  An URL in eBook to retrieve content from.
		 * @return Returns @c true if operation is success.
		 *
		 * @see MimeHelper
		 */
		virtual bool content( ContentData& data, const QUrl& browserUrl ) = 0;

		/**
		 * @brief Returns a title of topic by specified url.
		 *
		 * If the eBook has no table of contents or there is no page with the
		 * specified url, an empty page is returned.
		 */
		virtual QString topicTitle( const QUrl& browserUrl ) = 0;
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_CONTENTPROVIDER_HPP
