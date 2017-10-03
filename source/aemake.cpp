#include <algorithm>
#include <fstream>
#include <string>
#include <string.h>
#include <map>

#define AEMAKE_PLATFORM_WINDOWS			0
#define AEMAKE_PLATFORM_LINUX			0
#define AEMAKE_PLATFORM_MACOS			0

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WIN64__)
#	undef 	AEMAKE_PLATFORM_WINDOWS
#	define 	AEMAKE_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#	undef 	AEMAKE_PLATFORM_LINUX
#	define 	AEMAKE_PLATFORM_LINUX 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#	undef 	AEMAKE_PLATFORM_MACOS
#	define 	AEMAKE_PLATFORM_MACOS 1
#endif
#if defined (_WIN32)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif // WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif // NOMINMAX
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

bool FileCopy(const std::string& _fromName, const std::string& _toName)
{
	std::ifstream  src(_fromName.c_str(), std::ios::binary);
	std::ofstream  dst(_toName.c_str(), std::ios::binary);

	dst << src.rdbuf();
	return false;
}

bool FileDelete(const std::string& _path)
{
#if AEMAKE_PLATFORM_WINDOWS
	return DeleteFileW(utf8toUtf16(_path).c_str()) != 0;
#else
	return remove(_path.c_str()) == 0;
#endif
}

int FileExists(const char* _path) {
	//	AE_ASSERT(!_path.IsEmpty());
#if AEMAKE_PLATFORM_WINDOWS
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

uint64_t FileSize(const char* _path)
{
#if AEMAKE_PLATFORM_WINDOWS
#	pragma warning( push )
#	pragma warning( disable : 4996)
#endif // AEMAKE_PLATFORM_WINDOWS
	FILE* file = fopen(_path, "rb");
#if AEMAKE_PLATFORM_WINDOWS
#	pragma warning( pop )
#endif // AEMAKE_PLATFORM_WINDOWS
	if (file) {
		fseek(file, 0, SEEK_END);
		uint64_t sizeWhole = ftell(file);
		fseek(file, 0, SEEK_SET); // seek back to beginning of file
		fclose(file);
		return sizeWhole;
	}
	else {
		return 0;
	}
}


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


class Mutex
{
public:
	Mutex() {
#if _WIN32
		handle = new CRITICAL_SECTION;
		InitializeCriticalSection((CRITICAL_SECTION*)handle);
#else
		handle = new pthread_mutex_t;
		pthread_mutex_t* mutex = (pthread_mutex_t*)handle;
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(mutex, &attr);
#endif
	}

	~Mutex() {
#if _WIN32
		CRITICAL_SECTION* cs = (CRITICAL_SECTION*)handle;
		DeleteCriticalSection(cs);
		delete cs;
		handle = NULL;
#else
		pthread_mutex_t* mutex = (pthread_mutex_t*)handle;
		pthread_mutex_destroy(mutex);
		delete mutex;
		handle = 0;
#endif
	}

	void Acquire() {
#if _WIN32
		EnterCriticalSection((CRITICAL_SECTION*)handle);
#else
		pthread_mutex_lock((pthread_mutex_t*)handle);
#endif
	}

	bool TryAcquire() {
#if _WIN32
		return TryEnterCriticalSection((CRITICAL_SECTION*)handle) != FALSE;
#else
		return pthread_mutex_trylock((pthread_mutex_t*)handle) == 0;
#endif
	}

	void Release() {
#if _WIN32
		LeaveCriticalSection((CRITICAL_SECTION*)handle);
#else
		pthread_mutex_unlock((pthread_mutex_t*)handle);
#endif
	}

private:
	void* handle;
};

class ScopedLock {
public:
	ScopedLock(Mutex& _mutex) :
		mutex(_mutex)
	{
		mutex.Acquire();
	}
	~ScopedLock()
	{
		mutex.Release();
	}
private:
	ScopedLock(const ScopedLock& _rhs);
	ScopedLock& operator =(const ScopedLock& _rhs);
	Mutex& mutex;
};

class Thread {
public:
#ifdef _WIN32

	static DWORD WINAPI ThreadFunctionStatic(void* data)
	{
		Thread* thread = static_cast<Thread*>(data);
		thread->ThreadFunction();
		return 0;
	}

#else

	static void* ThreadFunctionStatic(void* data)
	{
		Thread* thread = static_cast<Thread*>(data);
		thread->ThreadFunction();
		pthread_exit((void*)0);
		return 0;
	}

#endif

	Thread() :
		thandle(NULL),
		shouldRun(false)
	{}

	~Thread()
	{
		Stop();
	}

	virtual void ThreadFunction() = 0;

	bool Run()
	{
		if (thandle)
			return false;

		shouldRun = true;
#if _WIN32
		thandle = CreateThread(nullptr, 0, ThreadFunctionStatic, this, 0, nullptr);
#else
		thandle = new pthread_t;
		pthread_attr_t type;
		pthread_attr_init(&type);
		pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
		pthread_create((pthread_t*)thandle, &type, ThreadFunctionStatic, this);
#endif		
		return thandle != NULL;
	}

	void Stop() {
		if (!thandle)
			return;
		shouldRun = false;
#if _WIN32
		WaitForSingleObject((HANDLE)thandle, INFINITE);
		CloseHandle((HANDLE)thandle);
#else
		pthread_t* thread = (pthread_t*)thandle;
		if (thread)
			pthread_join(*thread, 0);
		delete thread;
#endif	
		thandle = NULL;
	}

protected:
	void* thandle;
	volatile bool shouldRun;
};

#ifndef __APPLE__
static const unsigned BUFFERSIZE = 4096;
#endif

class FileWatcher : public Thread
{
public:
	FileWatcher() :
		delay(1.0f)
	{
#if AEMAKE_PLATFORM_LINUX
		watchHandle = -1;
		watchHandle = inotify_init();
#elif AEMAKE_PLATFORM_MACOS
		_queueFD = -1;
		_dirFD = -1;
		_stopped = false;
#endif
	}

	~FileWatcher() {
		StopWatching();
#if AEMAKE_PLATFORM_LINUX
		close(watchHandle);
#elif AEMAKE_PLATFORM_MACOS
		close(_dirFD);
		close(_queueFD);
#endif
	}

	bool StartWatching(const std::string& _path) {
		StopWatching();
#if AEMAKE_PLATFORM_WINDOWS
		dirHandle = (void*)CreateFileW(
			utf8toUtf16(RemoveTrailingSlash(_path)).c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if (dirHandle != INVALID_HANDLE_VALUE){
			path = StringAppend(_path);
			Run();
			return true;
		}
		else {
			return false;
		}
#elif AEMAKE_PLATFORM_LINUX
		int flags = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE;
		int Handle = inotify_add_watch(watchHandle, RemoveTrailingSlash(_path).c_str(), flags);
		if (Handle == -1) {
			printf("Failed inotify: %s err:%d\n", _path.c_str(), errno);
			return false;
		}

		dirHandle[Handle] = "";
		path = StringAppend(_path);
		Run();
		return true;

#elif AEMAKE_PLATFORM_MACOS
		_dirFD = open(_path.c_str(), O_EVTONLY);
		if(_dirFD < 0){
			printf("Unable to open directory \n");
			return false;
		}
		_queueFD = kqueue();
		if (_queueFD < 0){
			close(_dirFD);
			printf("Cannot create kqueue \n");
			return false;
			//("Cannot create kqueue", errno);
		}
		return true;
#endif
	}

	void StopWatching() {
		if (thandle)
		{
			shouldRun = false;

#if AEMAKE_PLATFORM_WINDOWS
		std::string dummyFileName = path + "dummy.tmp";
		std::fstream fs;
		fs.open(dummyFileName, std::ios::out);
		fs.close();
		FileDelete(dummyFileName);
#endif

#if AEMAKE_PLATFORM_MACOS
		Stop();
#endif	

#if AEMAKE_PLATFORM_WINDOWS
			CloseHandle((HANDLE)dirHandle);
#elif AEMAKE_PLATFORM_LINUX
			for (std::map<int, std::string>::iterator i = dirHandle.begin(); i != dirHandle.end(); ++i)
				inotify_rm_watch(watchHandle, i->first);
			dirHandle.clear();
#elif AEMAKE_PLATFORM_MACOS
			_stopped = true;
			//CloseFileWatcher(watcher);
#endif	

#if !AEMAKE_PLATFORM_MACOS
			Stop();
#endif	
			path.clear();
		}
	}

	std::string GetPath() {
		return path;
	}

	void SetDelay(float _interval) {
		delay = std::max(_interval, 0.0f);
	}

	void ThreadFunction() {
#if AEMAKE_PLATFORM_WINDOWS
		unsigned char buffer[BUFFERSIZE];
		DWORD bytesFilled = 0;
		while (shouldRun) {
			if (ReadDirectoryChangesW((HANDLE)dirHandle,
				buffer,
				BUFFERSIZE,
				false,
				FILE_NOTIFY_CHANGE_FILE_NAME |
				FILE_NOTIFY_CHANGE_DIR_NAME |
				FILE_NOTIFY_CHANGE_CREATION |
				FILE_NOTIFY_CHANGE_LAST_WRITE,
				&bytesFilled,
				nullptr,
				nullptr))
			{
				unsigned offset = 0;

				while (offset < bytesFilled)
				{
					const FILE_NOTIFY_INFORMATION* record = (FILE_NOTIFY_INFORMATION*)&buffer[offset];
					switch (record->Action) {
					case FILE_ACTION_ADDED | FILE_ACTION_MODIFIED: {
						const wchar_t* src = record->FileName;
						const size_t end = record->FileNameLength / 2;
						std::wstring fileName(record->FileName, (record->FileNameLength / 2)+2);
						std::wstring tmp = fileName;
						std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
						AddChange(converter.to_bytes(tmp));
					}
					break;
				//	default:
				//		printf("unhandled action %d\n", record->Action);
					}

					if (!record->NextEntryOffset)
						break;
					else
						offset += record->NextEntryOffset;

					/*			FILE_NOTIFY_INFORMATION* record = (FILE_NOTIFY_INFORMATION*)&buffer[offset];
								std::cout << record->Action << std::endl;
								if (record->Action == FILE_ACTION_MODIFIED || record->Action == FILE_NOTIFY_CHANGE_SIZE || record->Action == FILE_ACTION_RENAMED_NEW_NAME)
								{
									std::string fileName;
									const wchar_t* src = record->FileName;
									const wchar_t* end = src + record->FileNameLength / 2;
									while (src < end) {
										std::wstring tmp = src;
										std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
										fileName.append(converter.to_bytes(tmp));
									}
									fileName = fileName;
								//	fileName = GetInternalPath(fileName);
									AddChange(fileName);
								}
								if (!record->NextEntryOffset)
									break;
								else
									offset += record->NextEntryOffset;
							*/
				}
			}
		}
#elif AEMAKE_PLATFORM_LINUX
		unsigned char buffer[BUFFERSIZE];
		while (shouldRun) {
			int i = 0;
			int length = (int)read(watchHandle, buffer, sizeof(buffer));

			if (length < 0) return;

			while (i < length) {
				inotify_event* ev = (inotify_event*)&buffer[i];

				if (ev->len > 0) {
					if (ev->mask & IN_MODIFY || ev->mask & IN_MOVE) {
						AddChange(dirHandle[ev->wd] + ev->name);
					}
				}
				i += sizeof(inotify_event) + ev->len;
			}
		}
#elif AEMAKE_PLATFORM_MACOS
		while (!_stopped)
		{
			struct timespec timeout;
			timeout.tv_sec = 0;
			timeout.tv_nsec = 200000000;
			unsigned eventFilter = NOTE_WRITE;
			struct kevent event;
			struct kevent eventData;
			EV_SET(&event, _dirFD, EVFILT_VNODE, EV_ADD | EV_CLEAR, eventFilter, 0, 0);
			int nEvents = kevent(_queueFD, &event, 1, &eventData, 1, &timeout);
			if (nEvents < 0 || eventData.flags == EV_ERROR)
			{
	/*			try
				{
					FileImpl::handleLastErrorImpl(owner().directory().path());
				}
				catch (Poco::Exception& exc)
				{
					owner().scanError(&owner(), exc);
				}*/
			}
			else if (nEvents > 0 || true)
			{
				ItemInfoMap newEntries;
	//			scan(newEntries);
	//			compare(entries, newEntries);
	//			std::swap(entries, newEntries);
	//			lastScan.update();
			}
		}
#endif
	}

	void AddChange(const std::string& fileName)
	{
		ScopedLock lock(mutex);
		changes[fileName].Reset();
	}

	bool GetNextChange(const std::string& dest) {
		ScopedLock lock(mutex);
		unsigned delayMsec = (unsigned)(delay * 1000.0f);

		if (changes.empty())
			return false;
		else {
			// We erase from the changes on every iteration.
			// This is sufficient for this case, where we only pull one item, which is the specific library.
			for (std::map<std::string, Timer>::iterator i = changes.begin(); i != changes.end(); ++i){
				if (i->second.GetMSec(false) >= delayMsec && dest == i->first) {
					changes.erase(i);
					return true;
				}
			}
			return false;
		}
	}

	class Timer
	{
	public:
		Timer() {
			Reset();
		}

		unsigned GetMSec(bool reset){
			unsigned currentTime = Tick();
			unsigned elapsedTime = currentTime - startTime_;
			if (reset)
				startTime_ = currentTime;

			return elapsedTime;
		}

		void Reset() {
			startTime_ = Tick();
		}

		static unsigned Tick()
		{
#ifdef _WIN32
			return (unsigned)timeGetTime();
#elif __EMSCRIPTEN__
			return (unsigned)emscripten_get_now();
#else
			struct timeval time;
			gettimeofday(&time, NULL);
			return (unsigned)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
		}

	private:

		unsigned startTime_;
	};

private:
	std::string path;
	Mutex mutex;
	std::map<std::string, Timer> changes;
#if AEMAKE_PLATFORM_WINDOWS
	void * dirHandle;
#elif AEMAKE_PLATFORM_LINUX
	std::map<int, std::string> dirHandle;
	int watchHandle;
#elif AEMAKE_PLATFORM_MACOS		
	class ItemInfo
	{
	public:
		ItemInfo() :
			size(0)
			{}

		ItemInfo(const ItemInfo& _rhs) :
			path(_rhs.path),
			size(_rhs.size),
			lastModified(_rhs.lastModified)
		{}
	/*		
		explicit ItemInfo(const File& f):
			path(f.path()),
			size(f.isFile() ? f.getSize() : 0),
			lastModified(f.getLastModified())
		{}*/

	private:
		std::string path;
		uint64_t size;
		uint64_t lastModified;
	};
	typedef std::map< std::string, ItemInfo> ItemInfoMap;

	int _queueFD;
	int _dirFD;
	bool _stopped;	
#endif
	float delay;
};

#include <iostream>

void CheckChanges()
{

}

int main(int argc, const char** argv)
{
	lua_State* L;
	int z;

	L = luaL_newstate();
	luaL_openlibs(L);

	z = premake_init(L);

	bool shouldWatch = false;
	std::string path = "";
	for(int i = 0; i < argc; i++){
		const std::string tmp(argv[i]);
		if(tmp.find("--file")!=std::string::npos) {
			size_t loc = tmp.find_first_of('=');	
			path = tmp.substr(loc+1, tmp.size());
		}
		else if(strcmp(argv[i],"--watch")==0) shouldWatch = true;
	}

	if (z == OKAY) {
		z = premake_execute(L, argc, argv, "src/_premake_main.lua");
	}

	while(shouldWatch){
		CheckChanges();
	}
	//		if (z == OKAY) z = premake_execute(L, argc, argv, "src/_premake_main.lua");
	DirIter it(GetCurrentPath());
	while (it.HasNext())
	{
	//	std::cout << it.GetName() << std::endl;

		it.Next();
	}



	lua_close(L);
	return z;
}
