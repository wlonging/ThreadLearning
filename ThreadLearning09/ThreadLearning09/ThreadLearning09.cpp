// ThreadLearning09.cpp : future的其他成员函数
//

#include "pch.h"
#include <future>
#include <condition_variable> //需要引入头文件
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
using namespace std;

std::atomic<int> g_num = 0;

void cal() {
	for (int i = 0; i < 1000000; i++)
		//g_num++;  //计算正常
		g_num += 1;  //计算正常
		//g_num = g_num + 1;  //计算异常
	return;
}

//线程函数，模拟异步请求
int mythread() {
	cout << "thread start ... " << endl;
	std::chrono::milliseconds dura(2000);//线程休眠5s
	std::this_thread::sleep_for(dura);
	cout << "thread end ... " << endl;
	return 5;
}

int main() {
	//普通函数调用
	
	//std::future<int> ful = std::async(std::launch::deferred,mythread); //等待3s,deferred
	std::future<int> ful = std::async(mythread); //创建异步任务，这段代码不阻塞
	std::future_status status = ful.wait_for(std::chrono::seconds(3));//等待3s,ready
	//std::shared_future<int> sful(ful); //方式一：将一般的future转成shared_future可以get多次
	//std::shared_future<int> sful(ful.share()); //方式二：将一般的future转成shared_future可以get多次

	//std::future_status status = ful.wait_for(std::chrono::seconds(1));//等待6s,timeout
	if (status == std::future_status::timeout) {
		//超时
		cout << "线程执行超时" << endl;
		cout << "结果是：" << ful.get() << endl;
	}
	else if (status == std::future_status::deferred) {
		//延迟执行
		cout << "延迟执行" << endl;
		cout << "结果是：" << ful.get() << endl;
	}
	else {
		//执行完毕，返回结果
		cout << "结果是：" << ful.get() << endl;
	}
	
	//原子操作
	/*
	std::thread t1(cal);
	std::thread t2(cal);
	t1.join();
	t2.join();
	cout << "计算结果为：" << g_num << endl;
	*/

	return 0;
}