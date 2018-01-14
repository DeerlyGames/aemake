#pragma once

#include <QDialog>
#include "systemtray.h"

class QTimer;
class QMenu;
class QAction;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QPushButton;


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
	QMenu*				cogsMenu;
	QAction*			aboutAction;
	QAction*			quitAction;
	QVBoxLayout*		verticalLayout;
	QHBoxLayout*		horizontalLayout;
	QSpacerItem			*horizontalSpacer;
	QPushButton			*menubutton;
	QPushButton			*configurationsButton;
};