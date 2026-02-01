/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2026 Nick Egorrov, nicegorov@yandex.ru
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

#include <QJsonValue>
#include <QMetaObject>
#include <QPluginLoader>
#include <QStaticPlugin>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

// As long as qAsConst is used.
// IWYU pragma: no_include "type_traits"
// IWYU pragma: no_include <type_traits>
#if __cplusplus >= 201703L
	#define qAsConst(s) std::as_const(s)
#elif QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
	#define qAsConst(s) (s)
#endif

#include <ubrowser/builder.hpp>

namespace UBrowser { struct Settings; }

#include "pluginmanager.h"


#ifdef PRINT_DEBUG
	#undef PRINT_DEBUG
#endif

#if defined PRINT_DEBUG_ALL || defined PRINT_DEBUG_PLUGINMANAGER
	#include <QDebug>
	#define PRINT_DEBUG
#endif


Plugin::Plugin( const QStaticPlugin& plugin )
	: m_loader{ nullptr },
	  m_instance{ plugin.instance() },
	  m_info{},
	  m_iid{},
	  m_file{}
{
	m_info = plugin.metaData().value( "MetaData" ).toObject();
	m_iid = plugin.metaData().value( "IID" ).toString();
	m_file = QString( "static/" ) + plugin.instance()->metaObject()->className();

#ifdef PRINT_DEBUG
	qDebug() << "PluginManager: new plugin from QStaticPlugin";
	qDebug() << "    IID  = " << m_iid;
	qDebug() << "    file = " << m_file;
#endif
}

Plugin::Plugin( QPluginLoader* loader )
	: m_loader{ loader },
	  m_instance{ nullptr },
	  m_info{},
	  m_iid{},
	  m_file{}
{
	m_info = loader->metaData().value( "MetaData" ).toObject();
	m_iid = loader->metaData().value( "IID" ).toString();
	m_file = loader->fileName();

#ifdef PRINT_DEBUG
	qDebug() << "PluginManager: new plugin from QPluginLoader";
	qDebug() << "    IID =  " << m_iid;
	qDebug() << "    file = " << m_file;
#endif
}

Plugin::~Plugin()
{
}

QObject* Plugin::instance() const
{
	if ( m_loader == nullptr )
		return m_instance;
	else
		return m_loader->instance();
}

const QString& Plugin::file() const
{
	return m_file;
}

const QString& Plugin::iid() const
{
	return m_iid;
}

const QJsonObject& Plugin::info() const
{
	return m_info;
}


PluginManager::PluginManager( QObject* parent )
	: QObject{parent}
{
	const auto staticPlugins = QPluginLoader::staticPlugins();

	for ( auto plugin : staticPlugins )
		addStaticPlugin( plugin );
}

void PluginManager::addPlugins( const QStringList& files )
{
	for ( const QString& f : files )
	{
		QPluginLoader* loader = new QPluginLoader( f, this );
		Plugin p{ loader };
		m_plugins.push_back( p );
	}
}

QList<Plugin> PluginManager::plugins( const QString& iid, bool exactly ) const
{
	if ( iid.isEmpty() )
		return m_plugins;

	QList<Plugin> result{};

	for ( const auto& p : m_plugins )
	{
		if ( ( exactly && p.iid() == iid ) || ( !exactly && p.iid().startsWith( iid ) ) )
			result.push_back( p );
	}

	return result;
}

void PluginManager::addStaticPlugin( const QStaticPlugin& plugin )
{
	Plugin p{ plugin };
	m_plugins.push_back( p );
}

void PluginManager::setBrowserBuilder( const QString& file )
{
	UBrowser::Builder* builder = nullptr;

	for ( const auto& p : qAsConst( m_plugins ) )
	{
		if ( p.file() == file )
		{
			builder = qobject_cast<UBrowser::Builder*>( p.instance() );
			break;
		}
	}

	if ( builder == nullptr )
		return;

	m_browserBuilderList.insert( builder->kind(), builder );
}

UBrowser::Builder* PluginManager::browserBuilder( const QString& kind ) const
{
	return m_browserBuilderList.value( kind, nullptr );
}

void PluginManager::configureBrowsers( const UBrowser::Settings& settings )
{
	for ( auto b : qAsConst( m_browserBuilderList ) )
		b->configure( settings );
}
