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

#include <QObject>

#include <ubrowser/contentprovider.hpp>

#include <ubrowser/browser.hpp> // IWYU pragma: associated


#ifndef UBROWSER_DEFAULT_ZOOM_STEP
	#define UBROWSER_DEFAULT_ZOOM_STEP 0.1
#endif

#ifndef UBROWSER_DEFAULT_ZOOM_MIN
	#define UBROWSER_DEFAULT_ZOOM_MIN 0.2
#endif

#ifndef UBROWSER_DEFAULT_ZOOM_MAX
	#define UBROWSER_DEFAULT_ZOOM_MAX 5.0
#endif


UBrowser::Browser::Browser( ContentProvider::Ptr contentProvider, QObject* parent ) :
	QObject( parent ),
	m_contentProvider( contentProvider ),
	m_zoomFactorMin{UBROWSER_DEFAULT_ZOOM_MIN},
	m_zoomFactorMax{UBROWSER_DEFAULT_ZOOM_MAX},
	m_zoomFactorStep{UBROWSER_DEFAULT_ZOOM_STEP},
	m_autoScroll{-1}
{
}

UBrowser::Browser::~Browser()
{
}

UBrowser::ContentProvider::Ptr UBrowser::Browser::contentProvider() const
{
	return m_contentProvider;
}

void UBrowser::Browser::setContentProvider( ContentProvider::Ptr contentProvider )
{
	m_contentProvider = contentProvider;
}

void UBrowser::Browser::setZoomFactor( qreal zoom )
{
	qreal z = zoom;
	z = z < m_zoomFactorMin ? m_zoomFactorMin : z;
	z = z > m_zoomFactorMax ? m_zoomFactorMax : z;
	setZoomFactorImpl( z );
}

void UBrowser::Browser::zoomIncrease()
{
	setZoomFactor( zoomFactor() + m_zoomFactorStep );
}

void UBrowser::Browser::zoomDecrease()
{
	setZoomFactor( zoomFactor() - m_zoomFactorStep );
}

void UBrowser::Browser::setAutoScroll( int pos )
{
	if ( pos >= 0 )
	{
		m_autoScroll = pos;
	}
}

int UBrowser::Browser::autoScroll()
{
	return m_autoScroll;
}

void UBrowser::Browser::scrollAfterLoad()
{
	if ( m_autoScroll >= 0 )
	{
		setScrollTop( m_autoScroll );
		m_autoScroll = -1;
	}
}
