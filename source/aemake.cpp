#include <string>
#include <string.h>

#if defined (_WIN32)/*
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif // WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN */
#	include <Windows.h>
#	include <TimeAPI.h>
#	include <locale>
#	include <codecvt>
#	include <vector>

std::wstring utf8toUtf16(const std::string & str)
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
#pragma warning( push )
#pragma warning( disable : 4267)
	vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw runtime_error("Failed converting UTF-8 string to UTF-16");
#pragma warning( pop ) 
	return wstring(&buffer[0], charsConverted);
}
#else 
#	include <dirent.h>
#endif

#if defined(__linux__)
#	include <errno.h> // Perhaps not needed errno
#	include <fcntl.h>
#	include <linux/limits.h>
#	include <stdlib.h>
#	include <sys/inotify.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "premake.h"
#ifdef __cplusplus
}
#endif

std::string StringAppend(const std::string& _str) {
	std::string res = _str;
	if (!(res.length() == 0) && res.at(res.length() - 1) != '/')
		res.append("/");
	return res;
}

std::string StringReplace(const std::string& _str, const std::string& _before, const std::string& _after) {
	size_t pos = 0;
	std::string retString = _str;
	while ((pos = retString.find(_before.c_str(), pos)) != std::string::npos) {
		retString.replace(pos, _before.length(), _after.c_str());
		pos += _after.length();
	}
	return retString.c_str();
}

std::string RemoveTrailingSlash(const std::string& _str)
{
	std::string res = StringReplace(_str, "\\", "/");
	if (!(res.length() == 0) && res.at(res.length() - 1) == '/')
		res.resize(res.length() - 1);
	return res;
}

const char* GetCurrentPath() {
#if defined _WIN32
	wchar_t result_path[MAX_PATH];
	result_path[0] = 0;

	DWORD dwRet = GetCurrentDirectoryW(MAX_PATH, result_path);
	if (dwRet == 0)
	{
		// Handle an error condition.
		//printf("GetFullPathName failed (%d)\n", GetLastError());
	}
	wcscat_s(result_path, L"\\");

	/* Replace '\\' with / */
	int i = 0;
	while (result_path[i])
	{
		if (result_path[i] == L'\\')
			result_path[i] = L'/';
		i++;
	}
	std::wstring tmp = result_path;
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return _strdup(converter.to_bytes(tmp).c_str());
#else
	char result_path[PATH_MAX];
	result_path[0] = 0;
	char* resultError = 0;
	resultError = getcwd(result_path, PATH_MAX);
	if (!resultError) {

	}
	return strdup(result_path);
#endif
}

class RefCounted
{
public:
	RefCounted() :
		counter(0)
	{}

	void Duplicate() const {
		++counter;
	}

	void Release() const{
		if(--counter == 0) delete this;
	}

protected:
//	(ReferenceCounted)
	/// Destroys the RefCountedObject.	

	virtual ~RefCounted(){}

private:
	class AtomicInteger{
	public:
		AtomicInteger(){
			counter = 0;
		}

		explicit AtomicInteger(int _val){
			counter = _val;
		}

		~AtomicInteger(){}

		int GetValue() const{
#if _WIN32
			return counter;
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
			return counter.load();
#else
			return counter;
#endif
		}

		int operator ++ (){
#if _WIN32
			int res = InterlockedIncrement(&counter);
			return --res;
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
			return ++counter;
#else
			return __sync_add_and_fetch(&counter, 1);
#endif
		}

		int operator -- (){
#if _WIN32
			return InterlockedIncrement(&counter);
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
			return --counter;
#else
			return __sync_sub_and_fetch(&counter, 1);
#endif
		}
public:
		typedef int ValueType;
#if _WIN32
	typedef volatile LONG Type;
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
	typedef std::atomic<int> Type;
#else
	typedef int Type;
#endif
	Type counter;		
	};

	mutable AtomicInteger counter;	
};

class DirIter
{
public:
	DirIter() :
	pimpl(0)
	{}

	DirIter(const std::string& _dir) :
		path(_dir),
		pimpl(new Implementation(_dir))
	{
		path = pimpl->Get();
	//	file = path;
	}
	DirIter(const DirIter& _iter) :
		path(_iter.path),
		pimpl(_iter.pimpl)
	{
		if (pimpl)
		{
			pimpl->Duplicate();
		//	_file = _path;
		}
	}

	~DirIter(){
		if (pimpl) pimpl->Release();
	}

	bool HasNext() const{
		if (GetName().empty()) {
			return false;
		}
		return true;
	}
	
	DirIter& Next(){
		if(pimpl){
			path = pimpl->Next();
		}
		return *this;
	}

	bool IsFile() const{
		return true;
	}

	bool IsDirectory() const{
		return false;
	}

	const std::string& GetName() const{
		return path;
	}

	DirIter& operator = (const DirIter& _it){
		if (pimpl) pimpl->Release();
		pimpl = _it.pimpl;
		if (pimpl)
		{
			pimpl->Duplicate();
			path = _it.path;
			//file = path;
		}
		return *this;		
	}

	//DirIter& operator = (const std::string& _dir);

	bool operator == (const DirIter& _iterator) const{
		return GetName() == _iterator.GetName();		
	}

	bool operator != (const DirIter& _iterator) const{
		return GetName() != _iterator.GetName();
	}

private:
	std::string path;
#if _WIN32
class Implementation : public RefCounted
{
public:
	Implementation(const std::string& _path) :
		fh(INVALID_HANDLE_VALUE)
	{
	//	Directory::Create(_path);
		std::string findPath(_path.c_str());
		findPath.append("*");

		fh = FindFirstFileW(utf8toUtf16(findPath).c_str(), &fd);
		if (fh == INVALID_HANDLE_VALUE)
		{
		//	if (GetLastError() != ERROR_NO_MORE_FILES)
		//		File::handleLastError(path);
		}
		else
		{
			std::wstring fileName = fd.cFileName;
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
			current = converter.to_bytes(fileName);
			if (current == "." || current == "..")
				Next();
		}
	}

	~Implementation(){
		if (fh != INVALID_HANDLE_VALUE)
			FindClose(fh);
	}

	const std::string& Get() const {
		return current;
	}

	const std::string& Next() {
		do{
			current.clear();
			if (FindNextFileW(fh, &fd) != 0)
			{
				std::wstring fileName = fd.cFileName;
				std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
				current = converter.to_bytes(fileName);
			}
		} while (current == "." || current == "..");
		return current;
	}

private:
	HANDLE           	fh;
	WIN32_FIND_DATAW 	fd;
	std::string 		current;
};
#else

class Implementation : public RefCounted
{
public:
	Implementation(const std::string& _path) :
		dir(0)
	{
		dir = opendir(_path.c_str());
		//	if (!_pDir) File::handleLastError(path);

		Next();
	}
	
	const std::string& Get() const {
		return current;
	}

	const std::string& Next() {
		do
		{
			struct dirent* pEntry = readdir(dir);
			if (pEntry)
				current = pEntry->d_name;
			else
				current.clear();
		}
		while (current == "." || current == "..");
		return current;
	}

private:
	DIR*        dir;
	std::string current;
};
	
#endif
	Implementation* pimpl;
};

#include <iostream>

int main(int argc, const char** argv)
{
	lua_State* L;
	int z;

	L = luaL_newstate();
	luaL_openlibs(L);

	z = premake_init(L);

	DirIter it(GetCurrentPath());
	while (it.HasNext())
	{
		std::cout << it.GetName() << std::endl;
		it.Next();
	}

	if (z == OKAY) {
	//	z = premake_execute(L, argc, argv, "src/_premake_main.lua");
	}

	lua_close(L);
	return z;
}
