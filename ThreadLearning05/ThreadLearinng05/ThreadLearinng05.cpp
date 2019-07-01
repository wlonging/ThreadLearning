// ThreadLearinng05.cpp : unique_lock的使用
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
			std::unique_lock<std::mutex> ul(my_mutex);
			Msg.push_back(i);
			cout << "插入一条消息:" << i << endl;

			//my_mutex.lock();
			//std::unique_lock<std::mutex> ul(my_mutex, std::adopt_lock); //std::adopt_lock标记已经加锁,前面需要lock
			//std::unique_lock<std::mutex> ul(my_mutex, std::try_to_lock); //std::try_to_lock尝试加锁,前面不能先lock
			/*
			std::unique_lock<std::mutex> ul(my_mutex, std::defer_lock); //std::defer_lock初始化一个未加锁的mutex，其之前也不能先lock，否则会报异常
			if (ul.try_lock()) {  //判断是否拿到锁
				//拿到锁
				Msg.push_back(i);
			}
			else
			{
				//没有拿到锁
				cout << "写数据线程没有拿到锁" << endl;
			}
			*/

			/*
			std::unique_lock<std::mutex> ul(my_mutex); //演示所有权释放
			Msg.push_back(i);
			std::mutex * p_m = ul.release(); //接管的互斥量指针需要手动释放已加锁的互斥量
			p_m->unlock();
			*/

			/*
			std::unique_lock<std::mutex> ul(my_mutex); //演示所有权转移，需要使用移动语义
			std::unique_lock<std::mutex> ul2 = std::move(ul);
			Msg.push_back(i);
			ul2.unlock();
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
		std::unique_lock<std::mutex> ul(my_mutex);
		//延迟1s
		std::chrono::milliseconds dura(1000);
		std::this_thread::sleep_for(dura);
		if (!Msg.empty()) {
			//读取消息，读完删除
			command = Msg.front();
			Msg.pop_front();
			return true;
		}
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