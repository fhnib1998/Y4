#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btnAnh_clicked();

    void on_btnQuet_clicked();

    void on_btnThemsv_clicked();

    void on_menuThemsv_clicked();

    void on_menuQuetsv_clicked();

    void on_btnAnhsv_clicked();

    void updateSV();

    void reset();

    void resetQuet();

    void on_btnXoasv_clicked();

    void on_btnSuasv_clicked();

    void on_menuTTsv_clicked();

    void on_menudashboard_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
