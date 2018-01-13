/********************************************************************************
** Form generated from reading UI file 'maindialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINDIALOG_H
#define UI_MAINDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MainContext
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *configurations;
    QSpacerItem *horizontalSpacer;
    QPushButton *settings;
    QListView *listView;

    void setupUi(QDialog *MainContext)
    {
        if (MainContext->objectName().isEmpty())
            MainContext->setObjectName(QString::fromUtf8("MainContext"));
        MainContext->setWindowModality(Qt::WindowModal);
        MainContext->resize(420, 360);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainContext->sizePolicy().hasHeightForWidth());
        MainContext->setSizePolicy(sizePolicy);
        MainContext->setMinimumSize(QSize(420, 360));
        MainContext->setMaximumSize(QSize(420, 360));
        MainContext->setBaseSize(QSize(420, 360));
        MainContext->setWindowOpacity(1);
        MainContext->setStyleSheet(QString::fromUtf8(""));
        MainContext->setModal(true);
        verticalLayout_2 = new QVBoxLayout(MainContext);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(1, 1, 1, 0);
        configurations = new QPushButton(MainContext);
        configurations->setObjectName(QString::fromUtf8("configurations"));
        configurations->setFlat(true);

        horizontalLayout->addWidget(configurations);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        settings = new QPushButton(MainContext);
        settings->setObjectName(QString::fromUtf8("settings"));
        settings->setAutoFillBackground(false);
        settings->setIconSize(QSize(32, 32));
        settings->setAutoDefault(true);
        settings->setDefault(false);
        settings->setFlat(true);

        horizontalLayout->addWidget(settings);


        verticalLayout_2->addLayout(horizontalLayout);

        listView = new QListView(MainContext);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setFrameShape(QFrame::NoFrame);
        listView->setFrameShadow(QFrame::Plain);
        listView->setLineWidth(0);

        verticalLayout_2->addWidget(listView);


        retranslateUi(MainContext);

        QMetaObject::connectSlotsByName(MainContext);
    } // setupUi

    void retranslateUi(QDialog *MainContext)
    {
        MainContext->setWindowTitle(QApplication::translate("MainContext", "Antler Make", 0, QApplication::UnicodeUTF8));
        configurations->setText(QApplication::translate("MainContext", "Debug", 0, QApplication::UnicodeUTF8));
        settings->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainContext: public Ui_MainContext {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINDIALOG_H
