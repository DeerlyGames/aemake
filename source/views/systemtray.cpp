#include "systemtray.h"
#include "QAction"
#include "QCoreApplication"
#include "QIcon"
#include <QCloseEvent>


SystemTray::SystemTray(QWidget *parent) :
	QSystemTrayIcon(QIcon(":/resources/icons/antler-white.png"), parent)
//	trayTimer(new QTimer(0)),
{
#if _WIN32
	setIcon(QIcon(":/resources/icons/antler-white.png"));
#else 
 //   setIcon(QIcon("/home/michael/DeerlyGames/Abstraction/abstraction.png"));
#endif
	setToolTip("Antler Make - C://Hello");
	show();
}