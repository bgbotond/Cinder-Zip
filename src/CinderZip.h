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

		std::vector< ci::fs::path > getFiles( const ci::fs::path& path = ci::fs::path() ) const;
		std::vector< ci::fs::path > getDirectories( const ci::fs::path& path = ci::fs::path() ) const;
		ci::DataSourceRef           loadFile( const ci::fs::path& file ) const;
		bool                        hasFile( const ci::fs::path& file ) const;
		std::vector< ci::fs::path > saveFiles( const ci::fs::path& path ) const;

	protected:
		ZipArchive( const ci::fs::path& path, const std::string& password );

		bool makeCurrentFile( const ci::fs::path& file ) const;
		void load();

	protected:
		ci::fs::path mPath;
		std::string  mPassword;
		unzFile      mUnzip;
		FileInfoList mFileInfos;
	};

} // namespace mndl