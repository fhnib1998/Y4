#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "string"
#include "QPixmap"
#include "QFileDialog"
#include "QDir"
#include "QMessageBox"
#include "QtSql"
#include "QTimer"
#include "wiringSerial.h"
#import "wiringSerial.c"
#include "sinhvien.h"
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
    QPixmap qplogo(dir+"/img/khoi.png");
    ui->logo->setPixmap(qplogo.scaled(200,175,Qt::KeepAspectRatio));
    QPixmap qpmeomeo(dir+"/img/hoahong.jpg");
    ui->meomeo->setPixmap(qpmeomeo.scaled(400,400,Qt::KeepAspectRatio));
    db.setDatabaseName(dir+"/qlsinhvien.sqlite");
    QTimer *timer;
    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateSV()));
    timer->start(100);
    ui->stackedWidget->setCurrentIndex(3);
    ui->menuThemsv->setFlat(true);
    ui->menuQuetsv->setFlat(true);
    ui->menuTTsv->setFlat(true);
}

MainWindow::~MainWindow()
{
    serialPrintf(fd,"987");
    delete ui;
}


void MainWindow::on_btnAnh_clicked()
{
    //Mở giao diện chọn file.Thiết lập file_name bằng đường dẫn file đã chọn
    file_name = QFileDialog::getOpenFileName(this,"Chon anh","/home/pi/fhnib");
    //Hiển thị ảnh vào label anh
    QPixmap qpanh(file_name);
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}

void MainWindow::on_btnQuet_clicked()
{
    //Kiểm tra đã quét vân tay chưa
    if(flag == 1){
        QMessageBox::critical(this,"Cảnh báo","Đã quét vân tay!");
    //Kiểm tra đã nhập ID chưa
    }else if(ui->id->text()==""){
        QMessageBox::critical(this,"Cảnh báo","Chưa nhập ID!");
    }else{
        //Kiểm tra Id đã tồn tại chưa
        string cid = ui->id->text().toStdString();
        db.open();
        sql = QString::fromStdString("select hoten from SinhVien where id = '"+cid+"'");
        qry.prepare(sql);
        qry.exec();
        if(qry.next()){
            QMessageBox::critical(this,"Cảnh báo","Đã tồn tại ID");
            db.close();
        }else{
            //Gửi ID đến Arduino
            char sid[3];
            strcpy(sid,cid.c_str());
            serialPrintf(fd,sid);
            //serialFlush(fd);
            flag = 0;
            //Chờ quét vân tay xong
            while(flag == 0){
                //Chờ dữ liệu Arduino gửi đến
                if(serialDataAvail(fd)){
                    string buffers;
                    while(serialDataAvail(fd)){
                        buffers += serialGetchar(fd);
                    }
                    char buffs[3];
                    strcpy(buffs,buffers.c_str());
                    int check = atoi(buffs);
                    //Kiểm tra dữ liệu gửi đến. Nếu là "987" thì dừng vòng lặ while
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
    //Kiểm tra đã quét vân tay chưa
    if(flag == 0){
        QMessageBox::critical(this,"Cảnh báo","Chưa quét vân tay!");
    //Kiểm tra đã nhập họ tên chưa
    }else if(ui->hoten->text()==""){
        QMessageBox::critical(this,"Cảnh báo","Chưa Nhập họ tên!");
    //Kiểm tra đã nhập mã sinh viên chưa
    }else if(ui->masv->text()==""){
        QMessageBox::critical(this,"Cảnh báo","Chưa Nhập mã sinh viên!");
    }else{
        //Tạo đối tượng SinhVien sv và set các thuộc tính của sv
        SinhVien sv;
        sv.setHoten(ui->hoten->text().toStdString());
        sv.setMasv(ui->masv->text().toStdString());
        sv.setNgaysinh(ui->ngaysinh->text().toStdString());
        sv.setGioiTinh(ui->gioitinh->currentText().toStdString());
        sv.setAnh(file_name.toStdString());
        sv.setId(ui->id->text().toStdString());
        //Thêm sinh viên vào SQLite
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
    //Thiết lập title
    ui->title->setText("Thêm sinh viên");
    ui->widget->setStyleSheet("background-color:#00b2cc;");
    //Hiển thị trang 1 trong stackedWidget
    ui->stackedWidget->setCurrentIndex(0);
    //Hiển thị nền button Quét sinh viên và tắt nền các button còn lại
    ui->menuThemsv->setFlat(false);
    ui->menuQuetsv->setFlat(true);
    ui->menuTTsv->setFlat(true);
    ui->menudashboard->setFlat(true);
    reset();
    //Gửi chuỗi "987" đến arduino
    serialPrintf(fd,"987");
}

void MainWindow::on_menuQuetsv_clicked()
{
    //Thiết lập title
    ui->title->setText("Quét sinh viên");
    ui->widget->setStyleSheet("background:#f50057;");
    //Hiển thị trang 2 trong stackedWidget
    ui->stackedWidget->setCurrentIndex(1);
    //Hiển thị nền button Quét sinh viên và tắt nền các button còn lại
    ui->menuThemsv->setFlat(true);
    ui->menuQuetsv->setFlat(false);
    ui->menuTTsv->setFlat(true);
    ui->menudashboard->setFlat(true);
    serialPrintf(fd,"999");
    resetQuet();
}

void MainWindow::on_btnAnhsv_clicked()
{
    file_name = QFileDialog::getOpenFileName(this,"Chon anh","/home/pi/fhnib");
    QPixmap qpanh(file_name);
    ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}


void MainWindow::updateSV()
{
    //Dung timer 0,1s de nhan du lieu
    //Chờ và nhận dữ liệu từ Arduino
    if(serialDataAvail(fd)){
        string buffer = "";
        while (serialDataAvail(fd)) {
            buffer += serialGetchar(fd);
        }
        char buff[3];
        strcpy(buff,buffer.c_str());
        int iid=atoi(buff);
        string id = to_string(iid);
        //Kiểm tra trong sql có id vừa nhận từ Arduino
        db.open();
        sql = QString::fromStdString("select * from SinhVien where id = '"+id+"'");
        qry.prepare(sql);
        qry.exec();
        //Nếu có thì hiển thị thông tin sinh viên
        if(qry.next()){
            string strHoten = qry.value(0).toString().toStdString();
            char cHoten[20];
            strcpy(cHoten,qry.value(0).toString().toStdString().c_str());
            serialPrintf(fd,cHoten);
            ui->welcome->setText("Henno:"+qry.value(0).toString()+" (^_^)");
            ui->txtHoten->setText(qry.value(0).toString());
            ui->txtMasv->setText(qry.value(1).toString());
            ui->txtNgaySinh->setDate(QDate::fromString(qry.value(2).toString(),"dd/MM/yyyy"));
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
    //thiết lập Textedit hoten về null
    ui->hoten->setText("\0");
    //thiết lập Textedit masv về null
    ui->masv->setText("\0");
    //thiết lập Timeedit về ngày 12/03/1998
    ui->ngaysinh->setDate(QDate::fromString("12/03/1998","dd/MM/yyyy"));
    //thiết lập Textedit id về null
    ui->id->setText("\0");
    //thiết lập màu nền label fingerprint sang màu đỏ
    ui->fingerprint->setStyleSheet("background:red;border-radius:20px");
    //Thiết lập label qpanh về ảnh vit.jpg
    QPixmap qpanh(dir+"/img/vit.jpg");
    ui->anh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
    flag = 0;
}

//reset giao dien quet
void MainWindow::resetQuet(){
    ui->welcome->setText("Nhập vân tay!!!");
    ui->txtHoten->setText("\0");
    ui->txtMasv->setText("\0");
    ui->txtNgaySinh->setDate(QDate::fromString("12/03/1998","dd/MM/yyyy"));
    ui->txtId->setText("\0");
    QPixmap qpanh(dir+"/img/vit.jpg");
    ui->txtAnh->setPixmap(qpanh.scaled(125,125,Qt::KeepAspectRatio));
}


void MainWindow::on_btnXoasv_clicked()
{
    //Kiểm tra đã quét sinh viên chưa
    if(ui->txtId->text().toStdString() == ""){
        QMessageBox::critical(this,"???","Chưa quét vân tay!");
    }else{
        //Hỏi có chắc chắn xóa sinh viên
        QMessageBox::StandardButton reply = QMessageBox::question(this,"Xóa?","Xóa thật à?",QMessageBox::No | QMessageBox::Yes);
        //Nếu chọn Yes xóa sinh viên khỏi sql
        if(reply == QMessageBox::Yes){
            string id = ui->txtId->text().toStdString();
            char sid[3];
            strcpy(sid,id.c_str());
            serialPrintf(fd,sid);
            db.open();
            sql = QString::fromStdString("delete from SinhVien where id = '"+id+"'");
            qry.prepare(sql);
            qry.exec();
            QMessageBox::information(this,"Okena","Đã xóa sinh viên!");
            resetQuet();
        }
    }
}

void MainWindow::on_btnSuasv_clicked()
{
    //Kiểm tra đã quét sinh viên chưa
    if(ui->txtId->text().toStdString() == ""){
        QMessageBox::critical(this,"???","Chưa quét vân tay!");
    }else{
        //Tạo đối tượng SinhVien sv và thiết lập các thuộc tính của sv
        SinhVien sv;
        sv.setHoten(ui->txtHoten->text().toStdString());
        sv.setMasv(ui->txtMasv->text().toStdString());
        sv.setNgaysinh(ui->txtNgaySinh->text().toStdString());
        sv.setGioiTinh(ui->txtGioitinh->currentText().toStdString());
        sv.setAnh(file_name.toStdString());
        sv.setId(ui->txtId->text().toStdString());
        //Sửa sinh viên trong sqlite
        db.open();
        sql = QString::fromStdString("update SinhVien set hoten='"+sv.getHoten()+"',masv='"+sv.getMasv()+"',ngaysinh='"+sv.getNgaysinh()+"',gioitinh='"+sv.getGioiTinh()+"',anh='"+sv.getAnh()+"' where id='"+sv.getId()+"'");
        qry.prepare(sql);
        qry.exec();
        db.close();
        ui->welcome->setText("Welcome:"+QString::fromStdString(sv.getHoten()));
        QMessageBox::information(this,"Okena","Đã sửa sinh viên!");
    }
}

void MainWindow::on_menuTTsv_clicked()
{
    //Gửi đến Arduino chuỗi "987"
    serialPrintf(fd,"987");
    //Đổi màu và tên title
    ui->widget->setStyleSheet("background:#00bfa5;");
    ui->title->setText("Danh sách sinh viên");
    //Hiển thị trang 3 trong stackedWidget
    ui->stackedWidget->setCurrentIndex(2);
    //Hiển thị nền button Danh sách sinh viên và tắt nền các button còn lại
    ui->menuThemsv->setFlat(true);
    ui->menuQuetsv->setFlat(true);
    ui->menuTTsv->setFlat(false);
    ui->menudashboard->setFlat(true);
    //Hiển thị danh sách sinh viên vào tableView
    db.open();
    qrytv->prepare("select hoten as 'Họ tên',masv as 'Mã sinh viên',ngaysinh as 'Ngày sinh',gioitinh as 'Giới tính',id as 'ID' from SinhVien order by hoten ASC");
    qrytv->exec();
    model->setQuery(*qrytv);
    ui->tableView->setModel(model);
    db.close();
}

void MainWindow::on_menudashboard_clicked()
{
    //Gửi đến Arduino chuỗi "987"
    serialPrintf(fd,"987");
    //Tắt nền button "Thêm sinh viên"
    ui->menuThemsv->setFlat(true);
    //Tắt nền button "Quét sinh viên"
    ui->menuQuetsv->setFlat(true);
    //Tắt nền button "Danh sách sinh viên"
    ui->menuTTsv->setFlat(true);
    //Hiển thị nền button "Dashboard"
    ui->menudashboard->setFlat(false);
    //Hiển thị trang 3 trong stackedWidget
    ui->stackedWidget->setCurrentIndex(3);
    //Thiết lập màu nền title
    ui->widget->setStyleSheet("background:#8e24aa;");
    //Đổi tên title "Dashboard"
    ui->title->setText("Dashboard");
}
