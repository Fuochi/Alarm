/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "qextserialport.h"
#include "qextserialenumerator.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QextSerialPort port;
    QMessageBox error;

private slots:
    void on_pushButton_close_clicked();
    void on_pushButton_open_clicked();
    void on_pushButton_Sens_1_clicked();
    void on_pushButton_Sens_2_clicked();
    void on_pushButton_Sens_3_clicked();
    void on_pushButton_Sens_4_clicked();
    void on_pushButton_Sens_5_clicked();
    void on_pushButton_Sens_6_clicked();
    void on_alarmButton_clicked();
    void receive();
    void on_btn_clearText_clicked();
};

#endif // MAINWINDOW_H
