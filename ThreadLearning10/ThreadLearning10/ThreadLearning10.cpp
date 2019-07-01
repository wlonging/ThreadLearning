// ThreadLearning10.cpp : async不确定性问题
//

#include "pch.h"
#include <future>
#include <condition_variable> //需要引入头文件
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
using namespace std;

//线程函数，模拟异步请求
int mythread() {
	cout << "thread start ... " << endl;
	std::chrono::milliseconds dura(3000);//线程休眠3s
	std::this_thread::sleep_for(dura);
	cout << "thread end ... " << endl;
	return 5;
}

int main() {
	//普通函数调用
	//std::future<int> ful = std::async(std::launch::deferred,mythread); //等待6s,deferred
	std::future<int> ful = std::async(mythread); //创建异步任务，这段代码不阻塞
	std::future_status status = ful.wait_for(0s);//等待0s,ready
	if (status == std::future_status::deferred) {
		//延迟执行
		cout << "线程执行超时" << endl;
		cout << ful.get() << endl;
	}
	else if (status == std::future_status::timeout) {
		//超时
		cout << "超时" << endl;
		cout << ful.get() << endl;
	}
	else {
		//执行完毕，返回结果
		cout << "结果是：" << ful.get() << endl;
	}
	return 0;
}