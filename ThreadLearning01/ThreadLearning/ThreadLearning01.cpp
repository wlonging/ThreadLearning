// ThreadLearning.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <thread>

using namespace std;

void myprint() {
	cout << "Thread starting..." << endl;
	//Other Operating
	cout << "Thread End..." << endl;
}

//使用类对象（可调用对象）创建线程
class TA {
public:
	int &m_i; //不建议在线程中使用引用和指针，这样detach时，主线程结束，相关变量被释放，导致子线程中使用不可预知的值
	TA(int &i) : m_i(i) {
		cout << "构造函数开始执行" << endl;
	}
	TA(const TA &ta) : m_i(ta.m_i) {
		cout << "拷贝构造函数开始执行" << endl;
	}
	~TA() {
		cout << "析构函数开始执行" << endl;
	}
	void operator()() {   //不能带参数
		cout << "子线程开始" <<"线程id:"<< std::this_thread::get_id()<< endl;
		//...子线程操作
		cout << "子线程结束" << endl;
	}
};



int main() {

	cout << "主线程id:" << std::this_thread::get_id() << endl;
	//执行两个操作，1.新建一个线程，入口为myprint 2. 执行线程
	//1. 普通函数创建线程
	//std::thread td(myprint);

	//2. 使用类对象（可调用对象）创建线程
	
	/*
	int a = 5;
	TA ta(a);
	std::thread td(ta);
	*/

	//3.使用lambda表达式创建线程
	auto mylbtd = []{
		cout << "线程id:" << std::this_thread::get_id() << endl;
		 cout<< "子线程开始" <<endl;
		 //...子线程操作
		 cout << "子线程结束" << endl;
	};
	std::thread td(mylbtd);

	if (td.joinable()) {  //判断线程能否join或detach
		td.join();
	}
	//join()会阻塞线程，在子线程执行完毕之后，在与主线程汇合
	//如果没有join，那么主线程结束而子线程未结束，则会报错
	//td.detach(); //一旦detach则不能再join
	cout << "主线程结束" << endl;
	return 0;
}
