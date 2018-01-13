#pragma once

#include <QSystemTrayIcon>
#include <QString>
#include <QTimer>
#include <QMouseEvent>

class SystemTray : public QSystemTrayIcon{
	Q_OBJECT
public:
	explicit SystemTray(QWidget *parent = 0);
//	~SystemTray();

//public slots:
//	void closeTray();
//	void openDialog();
//	void trayClicked(QSystemTrayIcon::ActivationReason e);

//	void mousePressEvent( QMouseEvent* ev);
};