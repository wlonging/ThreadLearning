// ThreadLearning11.cpp : 其它互斥量
//

#include "pch.h"
#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <list>
using namespace std;

class A {
public:
	//插入消息，模拟消息不断产生
	void insertMsg() {
		for (int i = 0; i < 10000; i++) {
			cout << "插入一条消息:" << i << endl;
			std::chrono::milliseconds limitedtime(100); //100ms时间
			if (my_mutex.try_lock_for(limitedtime)) {//如果线程在100ms之内拿到了锁，则执行的操做
			//当前时间点往后推100ms，与上面的功能相同
			//if(my_mutex.try_lock_until(std::chrono::steady_clock::now()+limitedtime)){
				Msg.push_back(i);
				my_mutex.unlock(); //解锁不能忘记
			}
			else {
				//没有拿到锁，可以休眠一段时间
				std::chrono::milliseconds sleeptime(100); //100ms时间
				std::this_thread::sleep_for(sleeptime);
			}
		}
	}
	//读取消息
	void readMsg() {
		int curMsg;
		for (int i = 0; i < 10000; i++) {
			if (MsgLULProc(curMsg)) {
				//读出消息了
				cout << "消息已读出" << curMsg << endl;
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
		if (!Msg.empty()) {
			//读取消息，读完删除
			command = Msg.front();
			Msg.pop_front();
			my_mutex.unlock();   //语句2
			return true;
		}
		my_mutex.unlock();//语句3
		return false;
	}
private:
	std::list<int> Msg;  //消息变量
	std::timed_mutex my_mutex; //互斥量对象
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