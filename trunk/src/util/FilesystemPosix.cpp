#include "anki/util/Filesystem.h"
#include "anki/util/Exception.h"
#include "anki/util/Assert.h"
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <ftw.h>
#include <cerrno>

namespace anki {

//==============================================================================
bool fileExists(const char* filename)
{
	ANKI_ASSERT(filename);
	struct stat s;
	if(stat(filename, &s) == 0)
	{
		return S_ISREG(s.st_mode);
	}
	else
	{
		return false;
	}
}

//==============================================================================
bool directoryExists(const char* filename)
{
	ANKI_ASSERT(filename);
	struct stat s;
	if(stat(filename, &s) == 0)
	{
		return S_ISDIR(s.st_mode);
	}
	else
	{
		return false;
	}
}

//==============================================================================
static int rmDir(const char* fpath, const struct stat* sb, int typeflag,
	struct FTW* ftwbuf)
{
	(void)sb;
	int rv = remove(fpath);

	if(rv)
	{
		throw ANKI_EXCEPTION(strerror(errno) + ": " + fpath);
	}

	return rv;
}

void removeDirectory(const char* dir)
{
	if(nftw(dir, rmDir, 64, FTW_DEPTH | FTW_PHYS))
	{
		throw ANKI_EXCEPTION(strerror(errno) + ": " + dir);
	}
}

//==============================================================================
void createDirectory(const char* dir)
{
	if(directoryExists(dir))
	{
		return;
	}

	if(mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
	{
		throw ANKI_EXCEPTION(strerror(errno) + ": " + dir);
	}
}

//==============================================================================
void toNativePath(const char* path)
{
	ANKI_ASSERT(path);
}

} // end namespace anki