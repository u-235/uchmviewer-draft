
#include "qtwebenginebrowser.h"
#include "qtwebenginebuilder.h"


QString QtWebEngineBuilder::kind() const
{
	return "browser/html";
}

void QtWebEngineBuilder::configure( const UBrowser::Settings& settings )
{

}

void QtWebEngineBuilder::setDataDirectory( const QDir& dir )
{

}

UBrowser::Browser* QtWebEngineBuilder::createController( UBrowser::ContentProvider::Ptr contentProvider, QObject* parent )
{
	return new QtWebEngine::Browser( contentProvider, parent );
}

bool QtWebEngineBuilder::hasOption( UBrowser::Option option ) const
{
	switch ( option )
	{
	case UBrowser::OPTION_HIGH_LIGHT_SEARCH_RESULT:
	case UBrowser::OPTION_IMAGES:
	case UBrowser::OPTION_JAVA_SCRIPT:
	case UBrowser::OPTION_JAVA:
		return true;

	default:
		return false;
	}
}

bool QtWebEngineBuilder::hasFeature( UBrowser::Feature feature ) const
{
	switch ( feature )
	{
	case UBrowser::FEATURE_COPY:
	case UBrowser::FEATURE_PRINT:
	case UBrowser::FEATURE_SCROLL:
	case UBrowser::FEATURE_SEARCH:
	case UBrowser::FEATURE_ZOOM:
		return true;

	default:
		return false;
	}
}
