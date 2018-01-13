#pragma once

#include <QDialog>
#include "SystemTray.h"

namespace Ui
{
	class MainContext;
}

class MainDialog : public QDialog{
	Q_OBJECT
public:
	explicit MainDialog(QWidget *parent = 0);
	~MainDialog();

public slots:
	void closeTray();

	void trayClicked(QSystemTrayIcon::ActivationReason e);
	virtual void closeEvent (QCloseEvent *event);
	void openDialog();

	void setConfiguration();

	void loop();

	void openAbout();

private:
	void createWindowSpecs();
	SystemTray*			trayIcon;
	QTimer*				trayTimer;
	Ui::MainContext*	ui;
	QMenu*				cogsMenu;
	QAction*			aboutAction;
	QAction*			quitAction;
};