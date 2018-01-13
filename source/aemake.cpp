#include "utils.h"
#include "views/maindialog.h"
#include <string>
#include <string.h>
#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QTextStream>

int main(int argc, char ** argv)
{
	printf("argument count %d\n", argc);
	std::string inputFile;
	std::string action;
	bool watching = false;
	
	if(argc==1){
		watching = true;
	}else{
		for(int i = 1; i < argc; ++i){
			const char * arg = argv[i];
			if(strcmp(arg, "gmake")==0){
				action = "gmake";
				runPremake(argc, (const char**)argv);
			}
			if(strcmp(arg, "--help")==0){
				runPremake(argc, (const char**)argv);
				return 0;
			}
			if(strcmp(arg, "--file")==0){
		//		inputFile = argv[i+1];
			}
			if(strcmp(arg, "--watch")==0){
				watching = true;
			}
		}
	}
	
	if(inputFile.size() == 0){
		inputFile = "";
		inputFile += ".aeproj";
	}

//	runPremake(argc, (const char**)argv);

	readSettings();
	

	if(watching) {
		QApplication app(argc, argv);
		app.setQuitOnLastWindowClosed(false); // very important for file dialogs.
		app.setOrganizationDomain("com");
		app.setOrganizationName("DeerlyGames");
		app.setApplicationName("Antler");

		QFontDatabase::addApplicationFont(":/resources/fonts/Open Sans Light.ttf");
		
		QFile styleFile(":/resources/themes/deerlygames.qss");
		if (styleFile.open(QIODevice::ReadOnly))
		{
			QTextStream textStream(&styleFile);
			QString styleSheet = textStream.readAll();
			styleFile.close();
			app.setStyleSheet(styleSheet);
		}
		else {
			printf("stylefile not found\n");
		}

		app.processEvents();
		MainDialog dialog;
		QObject::connect(&app, SIGNAL(aboutToQuit()), &dialog, SLOT(closeTray()));	
		return app.exec();
	}
    
	return 0;
}