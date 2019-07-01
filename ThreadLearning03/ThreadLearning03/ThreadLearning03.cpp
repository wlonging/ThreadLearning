// ThreadLearning03.cpp : （三）多线程和数据共享
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
#include <list>

using namespace std;

void myprint(int i) {
	cout << "Thread "<< i <<"start..." << endl;
	//....
	cout << "Thread " << i <<"end ..." << endl;
	return;
}

//演示多线程数据共享所构造的消息对象
class A {
public:
	//插入消息，模拟消息不断产生
	void insertMsg() {
		for (int i = 0; i < 10000; i++) {
			cout << "插入一条消息:" << i << endl;
			Msg.push_back(i);  //语句1
		}
	}
	//读取消息
	void readMsg() {
		int curMsg;
		for (int i = 0; i < 10000; i++) {
			if (!Msg.empty()) {
				//读取消息，读完删除
				curMsg = Msg.front();  //语句2
				Msg.pop_front();
				cout << "消息已读出" << curMsg << endl;
			}
			else {
				//消息暂时为空
			}
		}
	}
private:
	std::list<int> Msg;  //消息变量
};

int main() {
	//1. 创建多个线程
	/*
	vector<thread> myThreads; //使用容器存储线程
	//创建10个线程
	for (int i = 0; i < 10; i++) {
		myThreads.push_back(thread(myprint, i));
	}
	//让主线程等待10个线程运行完成
	for (auto iter = myThreads.begin(); iter != myThreads.end(); ++iter) {
		iter->join();
	}
	*/

	//2. 数据共享（这段代码是演示多线程同时读写数据时，不做特殊处理会产生异常，解决方式见下一节）
	A a;
	//创建一个插入消息线程
	std::thread insertTd(&A::insertMsg, &a); //这里要传入引用保证是同一个对象
	//创建一个读取消息线程
	std::thread readTd(&A::readMsg, &a); //这里要传入引用保证是同一个对象
	insertTd.join();
	readTd.join();
	cout << "Main Thread end..." << endl;
	return 0;
}