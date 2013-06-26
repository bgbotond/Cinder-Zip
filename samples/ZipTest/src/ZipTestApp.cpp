#include "cinder/app/AppNative.h"
#include "CinderZip.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderProjectApp : public AppNative
{
public:
	void setup();
	void update();
	void draw();
private:
	mndl::ZipArchiveRef mZipArchive;
};

void CinderProjectApp::setup()
{
	fs::path zipName( ci::app::getAssetPath( "test.zip" ));

	mZipArchive = mndl::ZipArchive::create( zipName, "password" );

	std::vector< ci::fs::path > files = mZipArchive->getFiles();

	ci::DataSourceRef bufferPng = mZipArchive->loadFile( "bin/test.png" );
	ci::DataSourceRef bufferUni = mZipArchive->loadFile( "test.uni" );
	ci::DataSourceRef bufferTxt = mZipArchive->loadFile( "test.txt" );
}

void CinderProjectApp::update()
{
}

void CinderProjectApp::draw()
{
}

CINDER_APP_NATIVE( CinderProjectApp, RendererGl ) 
