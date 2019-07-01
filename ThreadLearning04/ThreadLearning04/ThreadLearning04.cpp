// ThreadLearning04.cpp : （五）互斥量与死锁
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>  //引入互斥量头文件

using namespace std;

class A {
public:
	//插入消息，模拟消息不断产生
	void insertMsg() {
		for (int i = 0; i < 10000; i++) {
			cout << "插入一条消息:" << i << endl;
			//1. 传统lock unlock写法
			my_mutex.lock();
			Msg.push_back(i);
			my_mutex.unlock();

			// 2. 在{}包裹内，lock_guard在{}结束时会自动析构，相当于unlock
			/*
			{
				std::lock_guard<std::mutex> lgmutex(my_mutex);
				Msg.push_back(i);
			}
			*/
		}
	}
	//读取消息
	void readMsg() {
		int MsgCom;
		for (int i = 0; i < 10000; i++) {
			if (MsgLULProc(MsgCom)) {
				//读出消息了
				cout << "消息已读出" << MsgCom << endl;
			}
			else {
				//消息暂时为空
				cout << "消息为空" << endl;
			}
		}
	}
	//加解锁代码
	bool MsgLULProc(int &command) {
		my_mutex.lock();   //语句1
		//std::lock_guard<std::mutex> lgmutex(my_mutex); //使用lock_guard代替lock
		if (!Msg.empty()) {
			//读取消息，读完删除
			command = Msg.front();
			Msg.pop_front();
			my_mutex.unlock();   //语句2 *使用lock_guard之后不需要自己手动释放锁
			return true;
		}
		my_mutex.unlock();//语句3 *使用lock_guard之后不需要自己手动释放锁
		return false;
	}
private:
	std::list<int> Msg;  //消息变量
	std::mutex my_mutex; //互斥量对象
};

int main() {
	A a;
	//创建一个插入消息线程
	std::thread insertTd(&A::insertMsg, &a); //这里要传入引用保证是同一个对象
	//创建一个读取消息线程
	std::thread readTd(&A::readMsg, &a); //这里要传入引用保证是同一个对象
	insertTd.join();
	readTd.join();
	return 0;
}


//死锁演示代码：程序在开始执行一段时间后，会出现卡住不动的现象，意味着出现了死锁

//class A {
//public:
//	//插入消息，模拟消息不断产生
//	void insertMsg() {
//		for (int i = 0; i < 10000; i++) {
//			cout << "插入一条消息:" << i << endl;
//			my_mutex1.lock(); //语句1
//			my_mutex2.lock(); //语句2
//			Msg.push_back(i);
//			my_mutex2.unlock();
//			my_mutex1.unlock();
//
//			//std::lock(my_mutex1, my_mutex2);//顺序无所谓
//			////加上adopt_lock参数可以使互斥量不再次进行lock()
//			//std::lock_guard<std::mutex> lgmutex1(my_mutex1, std::adopt_lock);
//			//std::lock_guard<std::mutex> lgmutex2(my_mutex2, std::adopt_lock);
//			//Msg.push_back(i);
//
//		}
//	}
//	//读取消息
//	void readMsg() {
//		int MsgCom;
//		for (int i = 0; i < 10000; i++) {
//			MsgCom = MsgLULProc(i);
//			if (MsgLULProc(MsgCom)) {
//				//读出消息了
//				cout << "消息已读出" << MsgCom << endl;
//			}
//			else {
//				//消息暂时为空
//				cout << "消息为空" << endl;
//			}
//		}
//	}
//	//加解锁代码
//	bool MsgLULProc(int &command) {
//		int curMsg;
//		my_mutex2.lock(); //语句1
//		my_mutex1.lock(); //语句2
//		//std::lock(my_mutex1, my_mutex2);//顺序无所谓
//		if (!Msg.empty()) {
//			//读取消息，读完删除
//			command = Msg.front();
//			Msg.pop_front();
//			
//			my_mutex1.unlock();
//			my_mutex2.unlock();
//			return true;
//		}
//		my_mutex1.unlock();
//		my_mutex2.unlock();
//		return false;
//	}
//private:
//	std::list<int> Msg;  //消息变量
//	std::mutex my_mutex1; //互斥量对象1
//	std::mutex my_mutex2; //互斥量对象2
//};
//
//int main() {
//	A a;
//	//创建一个插入消息线程
//	std::thread insertTd(&A::insertMsg, &a); //这里要传入引用保证是同一个对象
//	//创建一个读取消息线程
//	std::thread readTd(&A::readMsg, &a); //这里要传入引用保证是同一个对象
//	insertTd.join();
//	readTd.join();
//	return 0;
//}