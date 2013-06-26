#pragma once 

#include "cinder/DataSource.h"
#include "zipengine/unzip.h"

namespace mndl
{
	class ZipArchive
	{
	protected:
		struct FileInfo
		{
			ci::fs::path mPath;
			size_t       mCompressedSize;
			size_t       mUncompressedSize;
		};

		typedef std::vector< FileInfo > FileInfoList;

	public:
		ZipArchive( const ci::fs::path& path, const std::string& password );
		~ZipArchive();

		std::vector< ci::fs::path > getFiles();
		ci::DataSourceRef           open( const ci::fs::path& file );

	protected:
		bool makeCurrentFile( const ci::fs::path& file );
		void load();

	protected:
		ci::fs::path mPath;
		std::string  mPassword;
		unzFile      mUnzip;
		FileInfoList mFileInfos;
	};

} // namespace mndl