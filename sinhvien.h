#pragma once
#ifndef SINHVIEN_H
#define SINHVIEN_H
#include <iostream>
#include <string>
using namespace std;

class SinhVien
{
private:
    string hoten;
    string masv;
    string gioitinh;
    string ngaysinh;
    string anh;
    string id;
public:
    SinhVien();
    SinhVien(string ht, string masv, string gt, string ns, string a, string i);
    ~SinhVien();
    string getHoten();
    void setHoten(string ht);
    string getMasv();
    void setMasv(string sv);
    string getGioiTinh();
    void setGioiTinh(string gt);
    string getNgaysinh();
    void setNgaysinh(string ns);
    string getAnh();
    void setAnh(string a);
    string getId();
    void setId(string i);
};

#endif // SINHVIEN_H
