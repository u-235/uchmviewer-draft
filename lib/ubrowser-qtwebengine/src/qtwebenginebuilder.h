#ifndef QTWEBENGINEBUILDER_H
#define QTWEBENGINEBUILDER_H

#include <QObject>

#include <ubrowser/contentprovider.hpp>
#include <ubrowser/builder.hpp>
#include <ubrowser/export.hpp>


class UBROWSER_API_DECL QtWebEngineBuilder: public QObject, public UBrowser::Builder
{
		Q_OBJECT
		Q_PLUGIN_METADATA( IID UBROWSER_BUILDER_IID FILE "qtwebengine.json" )
		Q_INTERFACES( UBrowser::Builder )

	public:
		QString kind() const override;
		void configure( const UBrowser::Settings& settings ) override;
		void setDataDirectory( const QDir& dir ) override;
		UBrowser::Browser* createController(
		    UBrowser::ContentProvider::Ptr contentProvider,
		    QObject* parent ) override;
		bool hasOption( UBrowser::Option option ) const override;
		bool hasFeature( UBrowser::Feature feature ) const override;
};

#endif // QTWEBENGINEBUILDER_H

