extern "C"
{ 
	#include "premake.h" 
}
#include <string>
#include <string.h>
/*

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
	//	CheckChanges();
	}
	//		if (z == OKAY) z = premake_execute(L, argc, argv, "src/_premake_main.lua");


	lua_close(L);
	return z;
}
*/

#include <QApplication>
#include <QPushButton>


 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);

     QPushButton hello("Hello world!");
     hello.resize(100, 30);

     hello.show();
     return app.exec();
 }