#include "utils.h"
extern "C"
{ 
	#include "premake.h" 
}
#if _WIN32
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif // _CRT_SECURE_NO_WARNINGS
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif // WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
#	include <Windows.h>
#	include "Shlwapi.h"
#	include <locale>
#	include <codecvt>
#	include <vector>

static std::wstring utf8toUtf16(const std::string & str)
{
	using std::runtime_error;
	using std::string;
	using std::vector;
	using std::wstring;

	if (str.empty())
		return wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
		throw runtime_error("Failed converting UTF-8 string to UTF-16");
	vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw runtime_error("Failed converting UTF-8 string to UTF-16");
	return wstring(&buffer[0], charsConverted);
}

#else
#	include <sys/time.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <unistd.h>
#endif
#include <QDesktopServices>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>


uint64_t fileLastModified(const char* _path)
{
#if _WIN32	
	HANDLE hFile = CreateFileW(utf8toUtf16(_path).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
        printf("CreateFile failed with %d\n", GetLastError());
        return 0;
    }
	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC, stLocal;
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        return 0;
    CloseHandle(hFile);    
    FileTimeToSystemTime(&ftWrite, &stUTC);
    return ftWrite.dwHighDateTime;

#else	
	struct stat st;
	if (stat(_path, &st) == 0)
		return st.st_mtime;
	else
		return 0;
#endif
}

int fileExists(const char* _path) {
	//	AE_ASSERT(!_path.IsEmpty());
#if _WIN32
	DWORD attr = GetFileAttributesW(utf8toUtf16(_path).c_str());
	if (attr == 0xFFFFFFFF)
	{
		switch (GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
		case ERROR_NOT_READY:
		case ERROR_INVALID_DRIVE:
			return false;
		default:
			return false;
			//	handleLastErrorImpl(_path);
		}
	}
	return true;
#else
	struct stat st;
	return ((stat(_path, &st) != -1) && (st.st_mode & S_IFREG));
#endif
}

int runPremake(int argc, const char** argv){
	lua_State* L;
	int z;

	L = luaL_newstate();
	luaL_openlibs(L);

	z = premake_init(L);

	if (z == OKAY) {
		z = premake_execute(L, argc, argv, "src/_premake_main.lua");
	}

	lua_close(L);
	return z;

}

void intialSetup()
{
#if defined (_WIN32)

	QSettings settings(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/"+qApp->organizationName()+"/"+qApp->applicationName()+"/"+
						"/AntlerMake.ini", QSettings::IniFormat);
#else
	QSettings settings;
#endif

	const QString ndkPath = settings.value("Android NDK").value<QString>();

	const QString androidsdkPath = settings.value("Android SDK").value<QString>();
	const QString msBuildPath = settings.value("MSBuild").value<QString>();
	const QString minGWPath = settings.value("MinGW").value<QString>();
}

void readSettings()
{
	/*
	QSettings settings(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/DeerlyGames/Antler/AntlerMake.ini", QSettings::IniFormat);
	settings.setValue("Android_NDK", "A:/Android/android-ndk-r13b");
	const QString ndkPath = settings.value("Android_NDK").value<QString>();
	const QString androidsdkPath = settings.value("Android_SDK").value<QString>();
	const QString msBuildPath = settings.value("MSBuild").value<QString>();
	const QString minGWPath = settings.value("MinGW").value<QString>();
	*/
}
