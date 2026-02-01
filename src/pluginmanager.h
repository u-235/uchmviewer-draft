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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	class QStringList;
#else
	using QStringList = QList<QString>;
#endif

class QPluginLoader;
struct QStaticPlugin;

namespace UBrowser { class Builder; struct Settings; }


class Plugin
{
	public:
		Plugin( const QStaticPlugin& plugin );
		Plugin( QPluginLoader* loader );
		~Plugin();

		QObject* instance() const;
		const QString& file() const;
		const QString& iid() const;
		const QJsonObject& info() const;

	protected:
		QPluginLoader* m_loader;
		QObject* m_instance;
		QJsonObject m_info;
		QString m_iid;
		QString m_file;
};


class PluginManager : public QObject
{
		Q_OBJECT

	public:
		explicit PluginManager( QObject* parent = nullptr );

		/**
		 * @brief addPlugins
		 *
		 * @param files
		 */
		void addPlugins( const QStringList& files );

		/**
		 * @brief plugins
		 *
		 * @param iid
		 * @param exactly
		 * @return
		 */
		QList<Plugin> plugins( const QString& iid = QString(), bool exactly = false ) const;

		/**
		 * @brief setBrowserBuilder
		 *
		 * @param file
		 */
		void setBrowserBuilder( const QString& file );

		/**
		 * @brief browserBuilder
		 *
		 * @param kind
		 * @return
		 */
		UBrowser::Builder* browserBuilder( const QString& kind ) const;

		void configureBrowsers( const UBrowser::Settings& settings );

	protected:
		void addStaticPlugin( const QStaticPlugin& plugin );

		QList< Plugin > m_plugins;
		QMap< QString, UBrowser::Builder* > m_browserBuilderList;
};

#endif // PLUGINMANAGER_H
