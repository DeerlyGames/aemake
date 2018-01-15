#include "maindialog.h"
#include "../utils.h"
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QInputDialog>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QRegExp>
#include <fstream>
#include <sstream>
#include <iostream>

MainDialog::MainDialog(QWidget *parent, const QString& filePath) :
	QDialog(parent),
	trayIcon(new SystemTray(this)),
	trayTimer(new QTimer(0)),
//	ui(new Ui::MainContext),
	cogsMenu(new QMenu(this)),
	aboutAction(new QAction("About", this)),
	quitAction(new QAction("Quit", this)),
	verticalLayout(new QVBoxLayout(this)),
	horizontalLayout(new QHBoxLayout()),
	horizontalSpacer(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)),
	menubutton(new QPushButton(this)),
	configurationsButton(new QPushButton(this)),
	file(new QFile(filePath))
{
//	ui->setupUi(this);

//	ui->configurations->addAction(aboutAction);

//	ui->settings->setIcon(QIcon(":/resources/icons/antlerConsole.png"));
//	ui->settings->show();
	createWindowSpecs();
	
	trayTimer->setInterval(120);

	verticalLayout->setSpacing(0);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setContentsMargins(1, 1, 1, 0);

	configurationsButton->setFlat(true);
	configurationsButton->setObjectName(QString::fromUtf8("configurations"));
	horizontalLayout->addWidget(configurationsButton);
	horizontalLayout->addItem(horizontalSpacer);


	menubutton->setObjectName(QString::fromUtf8("settings"));
    menubutton->setAutoFillBackground(false);
	menubutton->setAutoDefault(true);
    menubutton->setDefault(false);
    menubutton->setFlat(true);
	
	horizontalLayout->addWidget(menubutton);

	verticalLayout->addItem(horizontalLayout);


	connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	loop();
	connect(trayTimer, SIGNAL(timeout()), this, SLOT(openDialog()));
	printf("file open\n");
/*	if (!file->open(QFile::ReadOnly | QFile::Text)) return;
	printf("textstream\n");
	QTextStream in(file);
	printf("readall\n");
	QString str = in.readAll();
	printf("printing\n", str.toStdString().c_str());*/

	std::ifstream f("aemake.aeproj");
    std::stringstream buffer;

    buffer << f.rdbuf();
    QString str =QString::fromStdString( buffer.str() );
	QRegExp rx("watching(\\s|\\n)*{(.|\\n)*?}");
	QStringList list;
	int pos = 0;

	while ((pos = rx.indexIn(str, pos)) != -1) {
		list << rx.cap(1);
		pos += rx.matchedLength();
	}
//	QStringList wathingBlock = str.split("watching(\\s|\\n)*{(.|\\n)*?}",QString::SkipEmptyParts );
	foreach(QString files, list)
	{
		std::cout << files.toStdString();
	}
//	std::cout << str.toStdString();
	
	//	aboutAction->setIcon(QIcon(QIcon(":/resources/icons/antlerConsole.png").pixmap(QSize(32,32))));
	//	connect(aboutAction,  SIGNAL(triggered()), this, SLOT(openAbout()));
	//	cogsMenu->addAction(aboutAction);

	//	connect(quitAction, SIGNAL(triggered()), this, SLOT(closeTray()));
	//	cogsMenu->addAction(quitAction);

	//	ui->settings->setMenu(cogsMenu);

	//	connect(ui->configurations, SIGNAL(triggered()), this, SLOT(setConfiguration()));
	
}

MainDialog::~MainDialog()
{
	delete file;
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

	setWindowModality(Qt::WindowModal);
	setFixedHeight(420);
	setBaseSize(QSize(420, 360));
	setFixedWidth(360);
	setWindowOpacity(1);

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

void MainDialog::visibility(bool _show)
{
	if(_show){
		show();
		trayIcon->show();
	}else{
		hide();
		trayIcon->hide();
	}
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