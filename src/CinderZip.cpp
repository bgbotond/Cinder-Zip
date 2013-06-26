#include "CinderZip.h"

using namespace ci;
using namespace std;


namespace mndl
{
	ZipArchiveRef ZipArchive::create( const fs::path& path, const std::string& password )
	{
		return ZipArchiveRef( new ZipArchive( path, password ) );
	}

	ZipArchive::ZipArchive( const fs::path& path, const std::string& password )
		: mPath( path )
		, mPassword( password )
		, mUnzip( 0 )
	{
		load();
	}

	ZipArchive::~ZipArchive()
	{
		if( mUnzip )
		{
			unzClose( mUnzip );
			mUnzip = 0;
		}
	}


	void ZipArchive::load()
	{
		if( ! mUnzip )
		{
			mUnzip = unzOpen( mPath.string().c_str() );

			for( int ret = unzGoToFirstFile( mUnzip ); ret == UNZ_OK; ret = unzGoToNextFile( mUnzip ) )
			{
				unz_file_info fileInfo;
				char fileName[1024];
				unzGetCurrentFileInfo( mUnzip, &fileInfo, fileName, 1024, 0, 0, 0, 0 );

				FileInfo info;

				info.mPath             = fileName;
				info.mCompressedSize   = static_cast< size_t >( fileInfo.uncompressed_size );
				info.mUncompressedSize = static_cast< size_t >( fileInfo.compressed_size   );

				mFileInfos.push_back( info );
			}
		}
	}

	bool ZipArchive::makeCurrentFile( const ci::fs::path& file )
	{
		if( ! mUnzip )
			return false;

		for( int ret = unzGoToFirstFile( mUnzip ); ret == UNZ_OK; ret = unzGoToNextFile( mUnzip ) )
		{
			char fileName[1024];
			unzGetCurrentFileInfo( mUnzip, 0, fileName, 1024, 0, 0, 0, 0 );

			if( file.string() == fileName )
				return true;
		}

		return false;
	}

	vector< fs::path > ZipArchive::getFiles()
	{
		vector< fs::path > files;

		for( auto it = mFileInfos.begin(); it != mFileInfos.end(); ++it )
		{
			if( it->mPath.has_filename() ) 
				files.push_back( it->mPath );
		}

		return files;
	}

	DataSourceRef ZipArchive::loadFile( const ci::fs::path& file )
	{
		if( mUnzip )
		{
			if( makeCurrentFile( file ) )
			{
				int ret;

				if( mPassword.empty() )
					ret = unzOpenCurrentFile( mUnzip );
				else
					ret = unzOpenCurrentFilePassword( mUnzip, mPassword.c_str() );

				if( ret == UNZ_OK )
				{
					unz_file_info fileInfo;
					unzGetCurrentFileInfo( mUnzip, &fileInfo, 0, 0, 0, 0, 0, 0 );

					ci::Buffer buffer( fileInfo.uncompressed_size );

					int ret2 = unzReadCurrentFile( mUnzip, buffer.getData(), buffer.getDataSize() );

					unzCloseCurrentFile( mUnzip );

					if( ret2 >= 0 )
						return DataSourceBuffer::create( buffer );
				}
			}
		}

		return DataSourceBuffer::create( Buffer() );
	}

} // namespace mndl