#include "cinder/DataTarget.h"

#include "CinderZip.h"

using namespace ci;
using namespace std;

namespace mndl
{
	ZipArchiveRef ZipArchive::create( const fs::path& path, const string& password )
	{
		return ZipArchiveRef( new ZipArchive( path, password ) );
	}

	ZipArchive::ZipArchive( const fs::path& path, const string& password )
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

	bool ZipArchive::makeCurrentFile( const fs::path& file ) const
	{
		if( ! mUnzip )
			return false;

		for( int ret = unzGoToFirstFile( mUnzip ); ret == UNZ_OK; ret = unzGoToNextFile( mUnzip ) )
		{
			char fileName[1024];
			unzGetCurrentFileInfo( mUnzip, 0, fileName, 1024, 0, 0, 0, 0 );

			if( file.compare( string( fileName ) ) == 0 )
				return true;
		}

		return false;
	}

	vector< fs::path > ZipArchive::getFiles( const fs::path& path ) const
	{
		vector< fs::path > files;

		for( auto it = mFileInfos.begin(); it != mFileInfos.end(); ++it )
		{
			// inside the zip we cannot use is_regular_file function
			if( it->mPath.filename().string() != "." )
			{
				if( ! path.empty() && it->mPath.string().find( path.string() ) != 0 )
					continue;

				files.push_back( it->mPath );
			}
		}

		return files;
	}

	vector< fs::path > ZipArchive::getDirectories( const fs::path& path ) const
	{
		vector< fs::path > directories;

		for( auto it = mFileInfos.begin(); it != mFileInfos.end(); ++it )
		{
			// inside the zip we cannot use is_directoy function
			if( it->mPath.filename().string() == "." )
			{
				if( ! path.empty() && it->mPath.string().find( path.string() ) != 0 )
					continue;

				directories.push_back( it->mPath );
			}
		}

		return directories;
	}

	DataSourceRef ZipArchive::loadFile( const fs::path& file ) const
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

					Buffer buffer( fileInfo.uncompressed_size );

					int ret2 = unzReadCurrentFile( mUnzip, buffer.getData(), buffer.getDataSize() );

					unzCloseCurrentFile( mUnzip );

					if( ret2 >= 0 )
						return DataSourceBuffer::create( buffer );
				}
			}
		}

		return DataSourceBuffer::create( Buffer() );
	}

	bool ZipArchive::hasFile( const fs::path& file ) const
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
					return true;
				}
			}
		}

		return false;
	}

	vector< fs::path > ZipArchive::saveFiles( const fs::path& path ) const
	{
		vector< fs::path > files = getFiles( path );
		vector< fs::path > savedFiles;

		for( auto it = files.begin(); it != files.end(); ++it )
		{
			fs::path filePath = *it;
			DataSourceRef dataSource = loadFile( filePath );

			fs::path savedFile = mPath.parent_path() / filePath;

			fs::path dirPath = savedFile.parent_path();
			fs::create_directories( dirPath );

			DataTargetPathRef dataTargetFile = writeFile( savedFile );
			dataTargetFile->getStream()->writeData( dataSource->getBuffer().getData(), dataSource->getBuffer().getDataSize());

			savedFiles.push_back( savedFile );
		}

		return savedFiles;
	}

} // namespace mndl