/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *textEdit_Status;
    QComboBox *comboBox_Interface;
    QLabel *label;
    QPushButton *pushButton_open;
    QPushButton *pushButton_close;
    QLCDNumber *lcdNumber_Dist;
    QLabel *label_light;
    QProgressBar *progressBar_Dist;
    QTableView *tableView;
    QPushButton *alarmButton;
    QPushButton *btn_clearText;
    QPushButton *pushButton_Sens_1;
    QPushButton *pushButton_Sens_2;
    QPushButton *pushButton_Sens_3;
    QPushButton *pushButton_Sens_4;
    QPushButton *pushButton_Sens_5;
    QPushButton *pushButton_Sens_6;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(650, 625);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        textEdit_Status = new QTextEdit(centralWidget);
        textEdit_Status->setObjectName(QStringLiteral("textEdit_Status"));
        textEdit_Status->setEnabled(true);
        textEdit_Status->setGeometry(QRect(20, 310, 611, 241));
        comboBox_Interface = new QComboBox(centralWidget);
        comboBox_Interface->setObjectName(QStringLiteral("comboBox_Interface"));
        comboBox_Interface->setGeometry(QRect(20, 30, 151, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 46, 13));
        pushButton_open = new QPushButton(centralWidget);
        pushButton_open->setObjectName(QStringLiteral("pushButton_open"));
        pushButton_open->setGeometry(QRect(180, 30, 75, 23));
        pushButton_close = new QPushButton(centralWidget);
        pushButton_close->setObjectName(QStringLiteral("pushButton_close"));
        pushButton_close->setEnabled(false);
        pushButton_close->setGeometry(QRect(260, 30, 75, 23));
        lcdNumber_Dist = new QLCDNumber(centralWidget);
        lcdNumber_Dist->setObjectName(QStringLiteral("lcdNumber_Dist"));
        lcdNumber_Dist->setGeometry(QRect(540, 100, 91, 23));
        lcdNumber_Dist->setFrameShape(QFrame::Box);
        lcdNumber_Dist->setFrameShadow(QFrame::Plain);
        lcdNumber_Dist->setSegmentStyle(QLCDNumber::Flat);
        label_light = new QLabel(centralWidget);
        label_light->setObjectName(QStringLiteral("label_light"));
        label_light->setGeometry(QRect(540, 80, 101, 20));
        progressBar_Dist = new QProgressBar(centralWidget);
        progressBar_Dist->setObjectName(QStringLiteral("progressBar_Dist"));
        progressBar_Dist->setGeometry(QRect(540, 130, 91, 171));
        progressBar_Dist->setMaximum(40);
        progressBar_Dist->setValue(0);
        progressBar_Dist->setTextVisible(false);
        progressBar_Dist->setOrientation(Qt::Vertical);
        progressBar_Dist->setInvertedAppearance(false);
        tableView = new QTableView(centralWidget);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setGeometry(QRect(20, 60, 511, 241));
        alarmButton = new QPushButton(centralWidget);
        alarmButton->setObjectName(QStringLiteral("alarmButton"));
        alarmButton->setGeometry(QRect(530, 30, 111, 27));
        btn_clearText = new QPushButton(centralWidget);
        btn_clearText->setObjectName(QStringLiteral("btn_clearText"));
        btn_clearText->setGeometry(QRect(400, 30, 111, 27));
        pushButton_Sens_1 = new QPushButton(centralWidget);
        pushButton_Sens_1->setObjectName(QStringLiteral("pushButton_Sens_1"));
        pushButton_Sens_1->setGeometry(QRect(440, 80, 71, 27));
        pushButton_Sens_2 = new QPushButton(centralWidget);
        pushButton_Sens_2->setObjectName(QStringLiteral("pushButton_Sens_2"));
        pushButton_Sens_2->setGeometry(QRect(440, 110, 71, 27));
        pushButton_Sens_3 = new QPushButton(centralWidget);
        pushButton_Sens_3->setObjectName(QStringLiteral("pushButton_Sens_3"));
        pushButton_Sens_3->setGeometry(QRect(440, 140, 71, 27));
        pushButton_Sens_4 = new QPushButton(centralWidget);
        pushButton_Sens_4->setObjectName(QStringLiteral("pushButton_Sens_4"));
        pushButton_Sens_4->setGeometry(QRect(440, 170, 71, 27));
        pushButton_Sens_5 = new QPushButton(centralWidget);
        pushButton_Sens_5->setObjectName(QStringLiteral("pushButton_Sens_5"));
        pushButton_Sens_5->setGeometry(QRect(440, 200, 71, 27));
        pushButton_Sens_6 = new QPushButton(centralWidget);
        pushButton_Sens_6->setObjectName(QStringLiteral("pushButton_Sens_6"));
        pushButton_Sens_6->setGeometry(QRect(440, 230, 71, 27));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 650, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "Port", 0));
        pushButton_open->setText(QApplication::translate("MainWindow", "Open", 0));
        pushButton_close->setText(QApplication::translate("MainWindow", "Close", 0));
        label_light->setText(QApplication::translate("MainWindow", "Distance [cm]", 0));
        alarmButton->setText(QApplication::translate("MainWindow", "Alarm Checked", 0));
        btn_clearText->setText(QApplication::translate("MainWindow", "Clear Text", 0));
        pushButton_Sens_1->setText(QApplication::translate("MainWindow", "ON/OFF", 0));
        pushButton_Sens_2->setText(QApplication::translate("MainWindow", "ON/OFF", 0));
        pushButton_Sens_3->setText(QApplication::translate("MainWindow", "ON/OFF", 0));
        pushButton_Sens_4->setText(QApplication::translate("MainWindow", "ON/OFF", 0));
        pushButton_Sens_5->setText(QApplication::translate("MainWindow", "ON/OFF", 0));
        pushButton_Sens_6->setText(QApplication::translate("MainWindow", "ON/OFF", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
