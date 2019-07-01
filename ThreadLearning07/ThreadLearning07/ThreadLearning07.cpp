// ThreadLearning07.cpp : 条件变量及其成员函数
//

#include "pch.h"
#include <condition_variable> //需要引入头文件
#include <mutex>
#include <thread>
#include <list>
#include <iostream>
using namespace std;

class A {
public:
	//插入消息，模拟消息不断产生
	void insertMsg() {
		for (int i = 0; i < 10000; i++) {
			std::unique_lock<std::mutex> myul(my_mutex); //加锁
			{
				cout << "插入一条消息:" << i << endl;
				Msg.push_back(i);
			}
			//插入消息之后通知
			my_cond.notify_one();
		}
	}
	//读取消息
	void readMsg() {
		int curMsg;
		while (true) {
			std::unique_lock<std::mutex> myul(my_mutex); //加锁
			//使用条件变量的wait()进行等待
			my_cond.wait(myul, [this]() {
				if (!Msg.empty()) { //消息列表不为空
					return true;
				}
				return false;
			});
			curMsg = Msg.front();
			Msg.pop_front();
			cout << "消息已读出" << curMsg << endl;
		}
	}
private:
	std::list<int> Msg;  //消息变量
	std::mutex my_mutex; //互斥量对象
	std::condition_variable my_cond; //条件变量
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