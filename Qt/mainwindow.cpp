

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include "QStandardItemModel"

QStandardItemModel *model;
QStandardItem *firstRow;
int alarm[6]={0};
QPalette* palette_red = new QPalette();
QPalette* palette_green = new QPalette();
int open[6]={1,2,3,4,5,6};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    palette_red->setColor(QPalette::Button,Qt::red);
    palette_green->setColor(QPalette::Button,Qt::red);

    // Get all available COM Ports and store them in a QList.
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    // Read each element on the list, but
    // add only USB ports to the combo box.
    for (int i = 0; i < ports.size(); i++) {
        if (ports.at(i).portName.contains("USB")){
            ui->comboBox_Interface->addItem(ports.at(i).portName.toLocal8Bit().constData());
        }
    }
    // Show a hint if no USB ports were found.
    if (ui->comboBox_Interface->count() == 0){
        ui->textEdit_Status->insertPlainText("No USB ports available.\nConnect a USB device and try again.");
    }
    // Init Tableview
    model = new QStandardItemModel(6,5,this); //6 Rows and 5 Columns
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("MOTE-ID")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Route-OK")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("RSSI-SUM")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("Path")));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString("Sensor")));
    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->setVisible(0);//.verticalHeader().setVisible(False);
    // Fill Table with default Values(IDs + Zero)
    for (int i=0;i<6;i++)
    {
        firstRow = new QStandardItem(QString::number(i+2));
        model->setItem(i,0,firstRow);
        firstRow = new QStandardItem(QString("0"));
        model->setItem(i,1,firstRow);
    }
}

// Modify the table
void setTable(int row, int column, QString string){
    firstRow = new QStandardItem(QString(string));
    model->setItem(row,column,firstRow);
}

void setRowGreen(int row, int column){
    model->setData(model->index(row, column), (QColor(Qt::green)), Qt::BackgroundRole);
}
void setRowRed(int row, int column){
    model->setData(model->index(row, column), (QColor(Qt::red)), Qt::BackgroundRole);
}
void setRowYellow(int row, int column){
    model->setData(model->index(row, column), (QColor(Qt::yellow)), Qt::BackgroundRole);
}


// Delete Alarm (Change Color Depending on Routing-State)
void resetTable(){
int i=0;
for (;i<6;i++)
{
    if(model->item(i,1)->text().toInt()==0)
    {setRowYellow(i, 0);}
    else
    {setRowGreen(i, 0);}
}
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_open_clicked()
{
    port.setQueryMode(QextSerialPort::EventDriven);
    port.setPortName("/dev/" + ui->comboBox_Interface->currentText());
    port.setBaudRate(BAUD115200);
    port.setFlowControl(FLOW_OFF);
    port.setParity(PAR_NONE);
    port.setDataBits(DATA_8);
    port.setStopBits(STOP_1);
    port.open(QIODevice::ReadWrite);

    if (!port.isOpen())
    {
        error.setText("Unable to open port!");
        error.show();
        return;
    }

    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(receive()));

    ui->pushButton_close->setEnabled(true);
    ui->pushButton_open->setEnabled(false);
    ui->alarmButton->setEnabled(false);
    ui->comboBox_Interface->setEnabled(false);
}

void MainWindow::on_pushButton_close_clicked()
{
    if (port.isOpen())port.close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    ui->comboBox_Interface->setEnabled(true);
}

void MainWindow::on_pushButton_Sens_1_clicked()
{ QObject::connect(&port, SIGNAL(open[0]), this, SLOT(transmit(int)));
    if(open[0]==101)
    {
    ui->pushButton_Sens_1->setPalette(*palette_green);
    port.write("ID 1 SENSOR ON");
    open[0]=1;
    }
    else
    {
     ui->pushButton_Sens_1->setPalette(*palette_red);
     port.write("ID 1 SENSOR OFF");
     open[0]=101;
    }
}

void MainWindow::on_pushButton_Sens_2_clicked()
{
    QObject::connect(&port, SIGNAL(open[1]), this, SLOT(transmit(int)));
        if(open[1]==102)
        {
        ui->pushButton_Sens_1->setPalette(*palette_green);
        port.write("ID 1 SENSOR ON");
        open[1]=2;
        }
        else
        {
         ui->pushButton_Sens_1->setPalette(*palette_red);
         port.write("ID 1 SENSOR OFF");
         open[1]=102;
        }
}


void MainWindow::on_pushButton_Sens_3_clicked()
{
    QObject::connect(&port, SIGNAL(open[2]), this, SLOT(transmit(int)));
        if(open[2]==103)
        {
        ui->pushButton_Sens_1->setPalette(*palette_green);
        port.write("ID 1 SENSOR ON");
        open[2]=3;
        }
        else
        {
         ui->pushButton_Sens_1->setPalette(*palette_red);
         port.write("ID 1 SENSOR OFF");
         open[2]=103;
        }
}


void MainWindow::on_pushButton_Sens_4_clicked()
{
    QObject::connect(&port, SIGNAL(open[3]), this, SLOT(transmit(int)));
        if(open[3]==104)
        {
        ui->pushButton_Sens_1->setPalette(*palette_green);
        port.write("ID 1 SENSOR ON");
        open[3]=4;
        }
        else
        {
         ui->pushButton_Sens_1->setPalette(*palette_red);
         port.write("ID 1 SENSOR OFF");
         open[3]=104;
        }
}


void MainWindow::on_pushButton_Sens_5_clicked()
{
    QObject::connect(&port, SIGNAL(open[4]), this, SLOT(transmit(int)));
        if(open[4]==105)
        {
        ui->pushButton_Sens_1->setPalette(*palette_green);
        port.write("ID 1 SENSOR ON");
        open[4]=5;
        }
        else
        {
         ui->pushButton_Sens_1->setPalette(*palette_red);
         port.write("ID 1 SENSOR OFF");
         open[4]=105;
        }
}


void MainWindow::on_pushButton_Sens_6_clicked()
{
    QObject::connect(&port, SIGNAL(open[5]), this, SLOT(transmit(int)));
        if(open[5]==106)
        {
        ui->pushButton_Sens_1->setPalette(*palette_green);
        port.write("ID 1 SENSOR ON");
        open[5]=6;
        }
        else
        {
         ui->pushButton_Sens_1->setPalette(*palette_red);
         port.write("ID 1 SENSOR OFF");
         open[5]=106;
        }
}




void MainWindow::on_btn_clearText_clicked()
{
    ui->textEdit_Status->setText("");
}

// Reset Alarm ---> Reset Colors and Distance
void MainWindow::on_alarmButton_clicked()
{
    ui->alarmButton->setEnabled(false);
    int i=0;
    for (;i<6;i++)
    {
        alarm[i]=0;
    }
    resetTable();
    ui->lcdNumber_Dist->display(0);
    ui->progressBar_Dist->setValue(0);
}


// Serial Receive from the Central-Mote
// Each Package ends with a newline ('\n')
// Search for different Package-Types (QT_ROUTE or QT_ALARM)
void MainWindow::receive()
{

    static QString str;
        char ch;
        while (port.getChar(&ch))
        {
            str.append(ch);
            if (ch == '\n')     // End of line, start decoding
            {
                str.remove("\n", Qt::CaseSensitive);
                ui->textEdit_Status->append(str);


                if (str.contains("QT_ROUTE"))
                {
                    QStringList list = str.split(QRegExp("\\s"));

                    qDebug() << "Str value: " << str;
                    if(!list.isEmpty()){
                        // Print Package-Data
                        qDebug() << "List size " << list.size();
                        for (int i=0; i < list.size(); i++){
                            qDebug() << "List value "<< i <<" "<< list.at(i);
                        }
                        // Extract Package-Data and fill Routing-Table
                        int row_index = list.at(1).toInt()-2;
                        setTable(row_index,0,list.at(1));
                        setTable(row_index,1,list.at(2));
                        setTable(row_index,2,list.at(3));
                        if(list.length()>3 && list.at(4).length() > 0){
                            setTable(row_index,3,list.at(4));
                        }else{
                            if(list.at(2).toInt()==1){
                                setTable(row_index,3,"direct");
                            }else{
                                setTable(row_index,3,"");
                            }

                        }
                        if(list.at(2).compare(("0")) && alarm[row_index]==0){
                           setRowGreen(row_index,0);
                    }else
                        {
                            if(alarm[row_index]==0)
                        {
                            setRowYellow(row_index,0);

                        }else
                            {
                              setRowRed(row_index,0);
                            }
                        }

                    }
                }


                if (str.contains("QT_ALARM"))
                {
                    //double value = 0;
                    QStringList list = str.split(QRegExp("\\s"));

                    qDebug() << "Str value: " << str;
                    if(!list.isEmpty()){
                        // Print Package-Data
                        qDebug() << "List size " << list.size();
                        for (int i=0; i < list.size(); i++){
                            qDebug() << "List value "<< i <<" "<< list.at(i);
                        }
                        // Alarm-Detected --> Show Distance and change the color to red
                        setRowRed(list.at(1).toInt()-2,0);
                        alarm[list.at(1).toInt()-2]=1;
                        ui->alarmButton->setEnabled(true);
                        ui->progressBar_Dist->setMaximum(100);
                        int value = list.at(2).toInt() * 256 + list.at(3).toInt();
                        ui->lcdNumber_Dist->display(value);
                        ui->progressBar_Dist->setValue(value);
                    }
                }

                this->repaint();    // Update content of window immediately
                str.clear();
            }
        }
}
void MainWindow::send()(int value)
{char *buf; //creation of a buffer
    *buf = value;
      port->write(buf); //send the buffer
}
