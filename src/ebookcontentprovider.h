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

#ifndef CONTENTPROVIDERIMPL_H
#define CONTENTPROVIDERIMPL_H

class QString;
class QTextCodec;
class QUrl;

#include <ebook.h>
#include <ubrowser/contentprovider.hpp>


class EbookContentProvider : public UBrowser::ContentProvider
{
	public:
		EbookContentProvider( EBook::Ptr ebook );
		~EbookContentProvider();

		EBook::Ptr ebook();

		QUrl convertUrlForEbook( const QUrl& browserUrl ) const override;

		QUrl convertUrlForBrowser( const QUrl& ebookUrl ) const override;

		bool isValidUrl( const QUrl& browserUrl ) const override;

		bool content( UBrowser::ContentData& data, const QUrl& browserUrl ) override;

		QString topicTitle( const QUrl& browserUrl ) override;

		void setTextCodec( QTextCodec* codec );

	protected:
		QString urlScheme() const;

		EBook::Ptr m_ebook;
		QTextCodec* m_textCodec;
};

#endif // CONTENTPROVIDERIMPL_H
