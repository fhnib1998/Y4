#include "sinhvien.h"
#include <iostream>
#include <string>
using namespace std;

SinhVien::SinhVien(){}

SinhVien::SinhVien(string ht, string sv, string gt, string ns, string a, string i){
    hoten = ht;
    masv = sv;
    gioitinh = gt;
    ngaysinh = ns;
    anh = a;
    id = i;
}

SinhVien::~SinhVien(){}

string SinhVien::getHoten(){
    return hoten;
}

void SinhVien::setHoten(string ht){
    hoten = ht;
}

string SinhVien::getNgaysinh(){
    return ngaysinh;
}

void SinhVien::setGioiTinh(string gt){
    gioitinh = gt;
}

string SinhVien::getGioiTinh(){
    return gioitinh;
}

void SinhVien::setNgaysinh(string ns){
    ngaysinh = ns;
}

string SinhVien::getMasv(){
    return masv;
}

void SinhVien::setMasv(string sv){
    masv = sv;
}

string SinhVien::getAnh(){
    return anh;
}

void SinhVien::setAnh(string a){
    anh = a;
}

string SinhVien::getId(){
    return id;
}

void SinhVien::setId(string i){
     id = i;
}
