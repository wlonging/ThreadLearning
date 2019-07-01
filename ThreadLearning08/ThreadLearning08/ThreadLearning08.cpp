// ThreadLearning08.cpp : 异步任务
//

#include "pch.h"
#include <future>
#include <condition_variable> //需要引入头文件
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
using namespace std;

int mythread(int a) {
	cout << "异步线程id为：" << std::this_thread::get_id() << endl;
	cout << "异步任务开始...，你将等待3s" << endl;
	//等待一个时间，模拟延迟
	std::this_thread::sleep_for(std::chrono::seconds(3));
	cout << "异步任务结束，结果已经返回..." << endl;
	return 5;
}

//演示promise获取线程中的某个值
void save_val(std::promise<int> &tmp) {
	//当前线程执行一些复杂的操作
	cout << "线程一开始执行" << endl;
	int num = 100;
	num++;
	num *= 1000;
	num = num << 2;
	tmp.set_value(num);
	return;
}

//获取线程中的值
void get_val() {
	cout << "线程二开始执行" << endl;
	std::promise<int> ps;
	std::thread t(save_val,std::ref(ps));
	t.join();
	std::future<int> ful = ps.get_future();
	cout << "线程二从线程一拿到的的值为：" << ful.get() << endl;
	return;
}

int main() {
	cout << "主线程id为：" << std::this_thread::get_id() << endl;
	//创建异步任务
	std::future<int> ful = async(mythread,10);
	cout << "异步任务返回值为：" << ful.get() << endl; //阻塞，等待mythread()返回
	
	//async传参
	//1. std::launch::deferred，延迟调用在wait或者get的时候调用，并不创建新的线程
	//std::future<int> ful = std::async(std::launch::deferred, mythread);

	//2. std::launch::async，调用async函数时，就开始创建新的线程
	//std::future<int> ful = std::async(std::launch::async, mythread);

	//3. std::launch::async | std::launch::deferred，默认参数，系统根据实际情况二选一
	//std::future<int> ful = std::async(std::launch::async | std::launch::deferred, mythread);
	//cout << ful.get() << endl;

	//packaged_task类模板,普通创建
	/*
	std::packaged_task<int(int)> pt(mythread); //将函数打包，作为线程入口函数的参数
	std::thread t1(std::ref(pt), 10);
	t1.join();
	*/
	//使用lambda表达式创建,不会创建新的线程
	/*
	std::packaged_task<int(int)> pt([](int param) {
		cout << "异步线程id为：" << std::this_thread::get_id() << endl;
		cout << "异步任务开始...，你将等待3s" << endl;
		//等待一个时间，模拟延迟
		std::this_thread::sleep_for(std::chrono::seconds(3));
		cout << "异步任务结束，结果已经返回..." << endl;
		return 5;
	});
	pt(100);
	*/

	//promise类模板,普通创建
	//std::thread t(get_val);
	//t.join();

	cout << "主线程结束" << endl;
	return 0;
}