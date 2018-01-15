#include "maindialogLegacy.h"
#include <QApplication>

MainDialogLegacy::MainDialogLegacy(QWidget *parent) :
	QDialog(parent),
	trayIcon(new SystemTray(this))
{
	trayIcon->show();
}

MainDialogLegacy::~MainDialogLegacy()
{
}

void MainDialogLegacy::closeTray()
{
	trayIcon->hide();
	qApp->quit();
}
