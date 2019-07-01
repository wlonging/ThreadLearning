// ThreadLearning02.cpp : (二)线程传参。

#include "pch.h"
#include <iostream>
#include <thread>
#include <string>

using namespace std;

void myprint(const int i,const string &pmybuf) {
	cout << i << endl;
	cout << pmybuf << endl;
}

int main() {
	int val = 100;
	int &qval = val;
	char mybuf[] = "this is a string";
	//std::thread td(myprint, val, mybuf); //这个mybuf可能会在主线程结束后再传入线程，是不安全的，可以多次运行程序，会发现传入的字符串会出现异常
	//解决方法是将其转为临时对象传入线程
	std::thread td(myprint, val, string(mybuf));
	td.detach();
	cout << "finished" << endl;
	return 0;
}