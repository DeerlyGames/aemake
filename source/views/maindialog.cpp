#include "maindialog.h"
#include "ui_maindialog.h"
#include "../utils.h"
#include <QAction>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QInputDialog>
#include <QMenu>
#include <QTimer>

MainDialog::MainDialog(QWidget *parent) :
	QDialog(parent),
	trayIcon(new SystemTray(this)),
	trayTimer(new QTimer(0)),
	ui(new Ui::MainContext),
	cogsMenu(new QMenu(this)),
	aboutAction(new QAction("About", this)),
	quitAction(new QAction("Quit", this))
{
	ui->setupUi(this);

	ui->configurations->addAction(aboutAction);

	ui->settings->setIcon(QIcon(":/resources/icons/antlerConsole.png"));
	ui->settings->show();
	createWindowSpecs();
	
	trayTimer->setInterval(120);

	connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	loop();
	connect(trayTimer, SIGNAL(timeout()), this, SLOT(openDialog()));

	aboutAction->setIcon(QIcon(QIcon(":/resources/icons/antlerConsole.png").pixmap(QSize(32,32))));
	connect(aboutAction,  SIGNAL(triggered()), this, SLOT(openAbout()));
	cogsMenu->addAction(aboutAction);

	connect(quitAction, SIGNAL(triggered()), this, SLOT(closeTray()));
	cogsMenu->addAction(quitAction);
	
	ui->settings->setMenu(cogsMenu);

//	connect(ui->configurations, SIGNAL(triggered()), this, SLOT(setConfiguration()));

}

MainDialog::~MainDialog()
{
}

void MainDialog::closeTray()
{
	trayIcon->hide();
	qApp->quit();
}

void MainDialog::closeEvent(QCloseEvent * event)
{
	printf("closeEvent\n");
//	trayTimer->stop();
	QWidget::closeEvent(event);
}

void MainDialog::createWindowSpecs()
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint );
	hide();

	QDesktopWidget deskWidget;

	setFixedHeight(420);
	setFixedWidth(360);

	setObjectName("MainContext"); // css styling name
}

void MainDialog::trayClicked(QSystemTrayIcon::ActivationReason e)
{
	trayTimer->start();
	
	if(e == QSystemTrayIcon::DoubleClick) {
		trayTimer->stop();
/*	
		if (Cion_ProjectFolder().isEmpty())
			Cion_openProject(this);
		else{

#if AE_PLATFORM_WINDOWS
		if (!winMainWindow) {
			winMainWindow = new WinMainWindow();
			winMainWindow->setGeometry(100, 100, 1024, 720);
		}
			winMainWindow->show();
#else
		if (mainWindow == NULL) {
			mainWindow = new MainWindow();
			mainWindow->show();
		}
			mainWindow->show();
#endif
	//		QDesktopServices::openUrl(QUrl(Cion_ProjectFolder()));
		}*/
		trayIcon->hide();
		trayIcon->show();
	}
}

void MainDialog::openDialog()
{
	if(isVisible())
		hide();
	else{
		const QPoint p = QCursor::pos();
		const QDesktopWidget deskWidget;
		const int taskHeight = deskWidget.availableGeometry().height();
		const int taskWidth = deskWidget.availableGeometry().width();
		show();
		raise();
		activateWindow();
		if (p.x()+(width()/2)-7 >= taskWidth) move(taskWidth-width()-7, taskHeight-height()-5);
		else move(p.x()-(width()/2), taskHeight-height()-5);
	}
	trayTimer->stop();
}

void MainDialog::loop()
{
	QPoint p = QCursor::pos();

	QDesktopWidget deskWidget;
	int taskHeight = deskWidget.availableGeometry().height();
	int taskWidth = deskWidget.availableGeometry().width();

	QTimer::singleShot(10, this, SLOT(loop()));
}

void MainDialog::openAbout()
{
}

void MainDialog::setConfiguration()
{/*
	QStringList items;
	items << "Debug" << "Release";
	bool ok;
	QString item = QInputDialog::getItem(this, "QInputDialog::getItem()",
                                         "Debug:", items, 0, false, &ok);
	if (ok && !item.isEmpty())
        ui->configurations->setText(item);*/
}