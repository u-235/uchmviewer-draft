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


#include <QDebug>
#include <QJsonValue>
#include <QStaticPlugin>
#include <QPluginLoader>
#include <QtGlobal>

#include <ubrowser/builder.hpp>

#include "pluginmanager.h"


PluginManager::PluginManager( QObject* parent )
	: QObject{parent}
{
	qDebug() << "[DEBUG] PluginManager";

	const auto staticPlugins = QPluginLoader::staticPlugins();

	for ( auto plugin : staticPlugins )
	{
		auto builder = qobject_cast<UBrowser::Builder*>( plugin.instance() );

		if ( builder )
		{
			Plugin p;
			p.builtin = true;
			p.instance = plugin.instance();
			p.info = plugin.metaData().value( "MetaData" ).toObject();
			m_builderPlugins.push_back( p );
			qDebug() << "    Builder: " << p.info.value( "name" ).toString();
		}
	}
}

UBrowser::Builder* PluginManager::browserBuilder()
{
	auto builder = qobject_cast<UBrowser::Builder*>( m_builderPlugins[0].instance );
	return builder;
}
