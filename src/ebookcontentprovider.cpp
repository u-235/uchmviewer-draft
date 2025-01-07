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

#include <QByteArray>
#include <QString>
#include <QTextCodec>
#include <QUrl>

#include <ebook.h>
#include <ubrowser/contentprovider.hpp>

#include "mimehelper.h"

#include "ebookcontentprovider.h"


EbookContentProvider::EbookContentProvider( EBook::Ptr ebook ) :
	m_ebook{ebook},
	m_textCodec{nullptr}
{
}

EbookContentProvider::~EbookContentProvider()
{
}

EBook::Ptr EbookContentProvider::ebook()
{
	return m_ebook;
}

QUrl EbookContentProvider::convertUrlForEbook( const QUrl& browserUrl ) const
{
	QUrl result{browserUrl};

	if ( result.scheme() == EBOOK_URL_SCHEME )
		result.setScheme( urlScheme() );

	return result;
}

QUrl EbookContentProvider::convertUrlForBrowser( const QUrl& ebookUrl ) const
{
	QUrl result{ebookUrl};
	result.setScheme( EBOOK_URL_SCHEME );
	return result;
}

bool EbookContentProvider::isValidUrl( const QUrl& browserUrl ) const
{
	const QString& scheme = browserUrl.scheme();
	return ( scheme == EBOOK_URL_SCHEME || scheme == urlScheme() ) &&
	       m_ebook->isSupportedUrl( convertUrlForEbook( browserUrl ) );
}

bool EbookContentProvider::content( UBrowser::ContentData& data, const QUrl& browserUrl )
{
	const QUrl& ebookUrl = convertUrlForEbook( browserUrl );

	if ( !m_ebook->getFileContentAsBinary( data.buffer, ebookUrl ) )
		return false;

	data.asUtf8.clear();

	QString encoding = m_ebook->currentEncoding();
	data.mime = MimeHelper::mimeType( ebookUrl, data.buffer );

	if ( data.mime.startsWith( "text" ) )
	{
		if ( m_textCodec != nullptr )
			data.asUtf8 = m_textCodec->toUnicode( data.buffer ).toUtf8();
		else
			data.asUtf8 = data.buffer;
	}

	return true;
}

QString EbookContentProvider::topicTitle( const QUrl& browserUrl )
{
	return m_ebook->getTopicByUrl( convertUrlForEbook( browserUrl ) );
}

void EbookContentProvider::setTextCodec( QTextCodec* codec )
{
	m_textCodec = codec;
}

QString EbookContentProvider::urlScheme() const
{
	return m_ebook->urlScheme();
}
