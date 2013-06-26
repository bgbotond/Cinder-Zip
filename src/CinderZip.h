#pragma once 

#include "cinder/DataSource.h"
#include "unzip.h"

namespace mndl
{
	typedef std::shared_ptr< class ZipArchive > ZipArchiveRef;

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
		static ZipArchiveRef create( const ci::fs::path& path, const std::string& password );

		~ZipArchive();

		std::vector< ci::fs::path > getFiles();
		ci::DataSourceRef           loadFile( const ci::fs::path& file );

	protected:
		ZipArchive( const ci::fs::path& path, const std::string& password );

		bool makeCurrentFile( const ci::fs::path& file );
		void load();

	protected:
		ci::fs::path mPath;
		std::string  mPassword;
		unzFile      mUnzip;
		FileInfoList mFileInfos;
	};

} // namespace mndl