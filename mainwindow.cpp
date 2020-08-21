#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "string"
#include "QPixmap"
#include "QFileDialog"
#include "QMessageBox"
#include <QtSql>
#include <QTimer>
#include <wiringSerial.h>
#import <wiringSerial.c>
#include <sinhvien.h>
using namespace std;
QString path = QDir::currentPath();
QString file_name;
int flag = 0;
int i;
int fd = serialOpen("/dev/ttyUSB0",9600);
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
QSqlQuery qry(db);
QString sql;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList ql;
    ql.append("Nam");
    ql.append("Nữ");
    ui->gioitinh->addItems(ql);
    ui->txtGioitinh->addItems(ql);
    QPixmap qpfp("/home/pi/fhnib/qtproject/Y4/img/fingerprint.svg");
    ui->fingerprint->setPixmap(qpfp.scaled(40,40,Qt::KeepAspectRatio));
    QPixmap qpanh("/home/pi/fhnib/qtproject/Y4/img/vit.jpg");
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
    db.setDatabaseName("/home/pi/fhnib/qtproject/Y4/qlsinhvien.sqlite");
    ui->stackedWidget->setCurrentIndex(0);
    QTimer *timer;
    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateSV()));
    timer->start(200);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnAnh_clicked()
{
    file_name = QFileDialog::getOpenFileName(this,"Chon anh","/home/pi/fhnib");
    QPixmap qpanh(file_name);
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}

void MainWindow::on_btnQuet_clicked()
{
    if(flag == 1){
        QMessageBox::critical(this,"Cảnh báo","Đã quét vân tay!");
    }else if(ui->id->text()==""){
        QMessageBox::critical(this,"Cảnh báo","Chưa nhập ID!");
    }else{
        string cid = ui->id->text().toStdString();
        char sid[3];
        strcpy(sid,cid.c_str());
        serialPrintf(fd,sid);
        //serialFlush(fd);
        db.open();
        sql = QString::fromStdString("select hoten from SinhVien where id = '"+cid+"'");
        qry.prepare(sql);
        qry.exec();
        if(qry.next()){
            QMessageBox::critical(this,"Cảnh báo","Đã tồn tại ID");
            db.close();
        }
        flag = 0;
        while(flag == 0){
            if(serialDataAvail(fd)){
                string buffers;
                while(serialDataAvail(fd)){
                    buffers += serialGetchar(fd);
                }
                char buffs[3];
                strcpy(buffs,buffers.c_str());
                int check = atoi(buffs);
                if(check == 987){
                    flag = 1;
                    ui->fingerprint->setStyleSheet("background:green;");
                }
            }
        }
    }
}

void MainWindow::on_btnThemsv_clicked()
{
    if(flag == 0){
        QMessageBox::critical(this,"Cảnh báo","Chưa quét vân tay!");
    }else if(ui->hoten->text()==""){
        QMessageBox::critical(this,"Cảnh báo","Chưa Nhập họ tên!");
    }else if(ui->masv->text()==""){
        QMessageBox::critical(this,"Cảnh báo","Chưa Nhập mã sinh viên!");
    }else{
        SinhVien sv;
        sv.setHoten(ui->hoten->text().toStdString());
        sv.setMasv(ui->masv->text().toStdString());
        sv.setNgaysinh(ui->ngaysinh->text().toStdString());
        sv.setGioiTinh(ui->gioitinh->currentText().toStdString());
        sv.setAnh(file_name.toStdString());
        sv.setId(ui->id->text().toStdString());
        db.open();
        sql = QString::fromStdString("insert into SinhVien (hoten,masv,ngaysinh,gioitinh,anh,id) values ('"+sv.getHoten()+"','"+sv.getMasv()+"','"+sv.getNgaysinh()+"','"+sv.getGioiTinh()+"','"+sv.getAnh()+"','"+sv.getId()+"')");
        qry.prepare(sql);
        qry.exec();
        db.close();
        QMessageBox::information(this,"Thêm thành công!","Đã thêm sinh viên!");
        reset();
    }
}


void MainWindow::on_menuThemsv_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->menuThemsv->setFlat(false);
    ui->menuQuetsv->setFlat(true);
    reset();
    serialPrintf(fd,"987");
}

void MainWindow::on_menuQuetsv_clicked()
{
    reset();
    ui->stackedWidget->setCurrentIndex(1);
    ui->menuThemsv->setFlat(true);
    ui->menuQuetsv->setFlat(false);
    serialPrintf(fd,"999");
}

void MainWindow::on_btnAnhsv_clicked()
{
    file_name = QFileDialog::getOpenFileName(this,"Chon anh","/home");
    QPixmap qpanh(file_name);
    ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}


void MainWindow::updateSV()
{
//    if(serialDataAvail(fd)){
//        string buffer = "";
//        while (serialDataAvail(fd)) {
//            buffer += serialGetchar(fd);
//        }
//        char buff[3];
//        strcpy(buff,buffer.c_str());
//        ui->welcome->setText(QString::fromStdString(buff));
//        int iid = atoi(buff);
//        db.open();
//        string id = to_string(iid);
//        db.open();
//        sql = QString::fromStdString("select * from SinhVien where id = '"+id+"'");
//        qry.prepare(sql);
//        qry.exec();
//        if(qry.next()){
//            ui->welcome->setText("Welcome:"+qry.value(0).toString());
//            ui->txtHoten->setText(qry.value(0).toString());
//            ui->txtMasv->setText(qry.value(1).toString());
//            ui->txtNgaySinh->setDate(QDate::fromString(qry.value(2).toString()));
//            if(qry.value(3).toString()=="Nam"){
//                ui->txtGioitinh->setCurrentIndex(0);
//            }else{
//                ui->txtGioitinh->setCurrentIndex(1);
//            }
//            ui->txtId->setText(qry.value(5).toString());
//            QPixmap qpanh(qry.value(4).toString());
//            ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
//        }
//        db.close();
//    }
}

void MainWindow::reset(){
    ui->hoten->setText("\0");
    ui->masv->setText("\0");
    ui->gioitinh->setCurrentIndex(0);
    ui->ngaysinh->setDate(QDate::fromString("01/01/2000"));
    ui->id->setText("\0");
    ui->fingerprint->setStyleSheet("background:red");
    QPixmap qpanh("/home/pi/fhnib/qtproject/Y4/img/vit.jpg");
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
    flag = 0;
}
