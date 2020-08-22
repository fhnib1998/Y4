#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "string"
#include "QPixmap"
#include "QFileDialog"
#include "QDir"
#include "QMessageBox"
#include <QtSql>
#include <QTimer>
#include <wiringSerial.h>
#import <wiringSerial.c>
#include <sinhvien.h>
using namespace std;
QString dir = QDir::currentPath();
QString file_name;
int flag = 0;
int i;
int fd = serialOpen("/dev/ttyUSB0",9600);
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
QSqlQuery qry(db);
QString sql;
QSqlQueryModel* model = new QSqlTableModel();
QSqlQuery* qrytv = new QSqlQuery(db);
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
    QPixmap qpfp(dir+"/img/fingerprint.svg");
    ui->fingerprint->setPixmap(qpfp.scaled(40,40,Qt::KeepAspectRatio));
    QPixmap qpanh(dir+"/img/vit.jpg");
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
    ui->tableView->setStyleSheet("background:#f8bbd0;");
    db.setDatabaseName(dir+"/qlsinhvien.sqlite");
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
        db.open();
        sql = QString::fromStdString("select hoten from SinhVien where id = '"+cid+"'");
        qry.prepare(sql);
        qry.exec();
        if(qry.next()){
            QMessageBox::critical(this,"Cảnh báo","Đã tồn tại ID");
            db.close();
        }else{
            char sid[3];
            strcpy(sid,cid.c_str());
            serialPrintf(fd,sid);
            //serialFlush(fd);
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
                        ui->fingerprint->setStyleSheet("background:green;border-radius:20px;");
                    }
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
        QMessageBox::information(this,"Okena","Đã thêm sinh viên!");
        reset();
    }
}


void MainWindow::on_menuThemsv_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->menuThemsv->setFlat(false);
    ui->menuQuetsv->setFlat(true);
    ui->menuTTsv->setFlat(true);
    reset();
    serialPrintf(fd,"987");
}

void MainWindow::on_menuQuetsv_clicked()
{
    resetQuet();
    ui->stackedWidget->setCurrentIndex(1);
    ui->menuThemsv->setFlat(true);
    ui->menuQuetsv->setFlat(false);
    ui->menuTTsv->setFlat(true);
    serialPrintf(fd,"999");
}

void MainWindow::on_btnAnhsv_clicked()
{
    file_name = QFileDialog::getOpenFileName(this,"Chon anh","/home/pi/fhnib");
    QPixmap qpanh(file_name);
    ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}


void MainWindow::updateSV()
{
    if(serialDataAvail(fd)){
        string buffer = "";
        while (serialDataAvail(fd)) {
            buffer += serialGetchar(fd);
        }
        char buff[3];
        strcpy(buff,buffer.c_str());
        int iid=atoi(buff);
        string id = to_string(iid);
        db.open();
        sql = QString::fromStdString("select * from SinhVien where id = '"+id+"'");
        qry.prepare(sql);
        qry.exec();
        if(qry.next()){
            string strHoten = qry.value(0).toString().toStdString();
            char cHoten[20];
            strcpy(cHoten,qry.value(0).toString().toStdString().c_str());
            serialPrintf(fd,cHoten);
            ui->welcome->setText("Welcome:"+qry.value(0).toString());
            ui->txtHoten->setText(qry.value(0).toString());
            ui->txtMasv->setText(qry.value(1).toString());
            ui->txtNgaySinh->setDate(QDate::fromString(qry.value(2).toString()));
            if(qry.value(3).toString()=="Nam"){
                ui->txtGioitinh->setCurrentIndex(0);
            }else{
                ui->txtGioitinh->setCurrentIndex(1);
            }
            ui->txtId->setText(qry.value(5).toString());
            file_name = qry.value(4).toString();
            QPixmap qpanh(file_name);
            ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
        }
        db.close();
    }
}

void MainWindow::reset(){
    ui->hoten->setText("\0");
    ui->masv->setText("\0");
    ui->gioitinh->setCurrentIndex(0);
    ui->ngaysinh->setDate(QDate::fromString("12/03/1998"));
    ui->id->setText("\0");
    ui->fingerprint->setStyleSheet("background:red,border-radius:20px");
    QPixmap qpanh(dir+"/img/vit.jpg");
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
    flag = 0;
}
void MainWindow::resetQuet(){
    ui->welcome->setText("Nhập vân tay!!!");
    ui->txtHoten->setText("\0");
    ui->txtMasv->setText("\0");
    ui->txtGioitinh->setCurrentIndex(0);
    ui->txtNgaySinh->setDate(QDate::fromString("12/03/1998"));
    ui->txtId->setText("\0");
    QPixmap qpanh(dir+"/img/vit.jpg");
    ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}

void MainWindow::on_btnXoasv_clicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,"Xóa?","Xóa thật à?",QMessageBox::No | QMessageBox::Yes);
    if(reply == QMessageBox::Yes){
        string id = ui->txtId->text().toStdString();
        db.open();
        sql = QString::fromStdString("delete from SinhVien where id = '"+id+"'");
        qry.prepare(sql);
        qry.exec();
        QMessageBox::information(this,"Okena","Đã xóa sinh viên!");
        resetQuet();
    }
}

void MainWindow::on_btnSuasv_clicked()
{
    SinhVien sv;
    sv.setHoten(ui->txtHoten->text().toStdString());
    sv.setMasv(ui->txtMasv->text().toStdString());
    sv.setNgaysinh(ui->txtNgaySinh->text().toStdString());
    sv.setGioiTinh(ui->txtGioitinh->currentText().toStdString());
    sv.setAnh(file_name.toStdString());
    sv.setId(ui->txtId->text().toStdString());
    db.open();
    sql = QString::fromStdString("update SinhVien set hoten='"+sv.getHoten()+"',masv='"+sv.getMasv()+"',ngaysinh='"+sv.getNgaysinh()+"',gioitinh='"+sv.getGioiTinh()+"',anh='"+sv.getAnh()+"' where id='"+sv.getId()+"'");
    qry.prepare(sql);
    qry.exec();
    db.close();
    ui->welcome->setText("Welcome:"+QString::fromStdString(sv.getHoten()));
    QMessageBox::information(this,"Okena","Đã sửa sinh viên!");
}

void MainWindow::on_menuTTsv_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->menuThemsv->setFlat(true);
    ui->menuQuetsv->setFlat(true);
    ui->menuTTsv->setFlat(false);
    db.open();
    qrytv->prepare("select hoten as 'Họ tên',masv as 'Mã sinh viên',ngaysinh as 'Ngày sinh',gioitinh as 'Giới tính',id as 'ID' from SinhVien order by hoten ASC");
    qrytv->exec();
    model->setQuery(*qrytv);
    ui->tableView->setModel(model);
    db.close();
}
