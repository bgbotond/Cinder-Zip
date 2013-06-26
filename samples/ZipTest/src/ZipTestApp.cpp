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
	mndl::ZipArchive* mZipArchive;
};

void CinderProjectApp::setup()
{
	fs::path zipName( ci::app::getAssetPath( "test.zip" ));

	mZipArchive = new mndl::ZipArchive( zipName, "password" );

	std::vector< ci::fs::path > files = mZipArchive->getFiles();

	ci::DataSourceRef bufferPng = mZipArchive->open( "bin/test.png" );
	ci::DataSourceRef bufferUni = mZipArchive->open( "test.uni" );
	ci::DataSourceRef bufferTxt = mZipArchive->open( "test.txt" );
}

void CinderProjectApp::update()
{
}

void CinderProjectApp::draw()
{
}

CINDER_APP_NATIVE( CinderProjectApp, RendererGl ) 
