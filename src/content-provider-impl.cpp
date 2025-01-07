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

#include <QByteArray>
#include <QString>
#include <QTextCodec>
#include <QtGlobal>

#include <ebook.h>
#include <ubrowser/content-provider.hpp>

#include "mimehelper.h"

#include "content-provider-impl.h"


ContentProviderImpl::ContentProviderImpl( EBook::Ptr ebook ) :
	m_ebook{ebook},
	m_textCodec{nullptr}
{
}

ContentProviderImpl::~ContentProviderImpl()
{
}

EBook::Ptr ContentProviderImpl::ebook()
{
	return m_ebook;
}

QString ContentProviderImpl::urlScheme() const
{
	return m_ebook->urlScheme();
}

QString ContentProviderImpl::topicTitle( const QUrl& browserUrl )
{
	return m_ebook->getTopicByUrl( convertUrlForEbook( browserUrl ) );
}

void ContentProviderImpl::setTextCodec( QTextCodec* codec )
{
	m_textCodec = codec;
}

bool ContentProviderImpl::ebookIsValidUrl( const QUrl& ebookUrl ) const
{
	return m_ebook->isSupportedUrl( ebookUrl );
}

bool ContentProviderImpl::ebookContent( UBrowser::ContentData& data, const QUrl& ebookUrl )
{
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
