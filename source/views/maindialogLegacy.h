#pragma once

#include <QDialog>
#include "systemtray.h"

class QMenu;

class MainDialogLegacy : public QDialog{
	Q_OBJECT
public:
	explicit MainDialogLegacy(QWidget *parent = 0);
	~MainDialogLegacy();
public slots:
	void closeTray();
private:	
	SystemTray*			trayIcon;
};