// TheadLearning06.cpp : 单例设计模式与数据共享
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>  //引入互斥量头文件
using namespace std;


std::mutex resource_mutex;// 资源互斥量
std::once_flag g_flag;  //标记

class MyCAS {
private:
	MyCAS() {};
	static void CreateInstance() {
		cout << "使用call_once创建的单例类" << endl;
		m_instance = new MyCAS();
		static CGClass cg;
	};
private:
	static MyCAS *m_instance;
public:
	static MyCAS* GetInstance() {
		if (m_instance == NULL) {
			std::unique_lock<std::mutex> ul(resource_mutex);
			if (m_instance == NULL) {
				m_instance = new MyCAS();
				static CGClass cg;
			}
		}
		return m_instance;
	};
	static MyCAS* GetInstance2() {
		std::call_once(g_flag,CreateInstance);
		return m_instance;
	}
	//单例类回收函数
	class CGClass{
	public:
		~CGClass() {
			if (MyCAS::m_instance != NULL) {
				delete MyCAS::m_instance;
				MyCAS::m_instance = NULL;
				cout << "单例类被回收" << endl;
			}
		}
	};
	
	void test() {
		cout << "测试函数" << endl;
	}
};

//初始化单例类实例
MyCAS * MyCAS::m_instance = NULL;

void getSingleClass() {
	cout << "创建单例类" << endl;
	MyCAS *p_a = MyCAS::GetInstance(); //获取单例类对象，最好在使用多线程之前加载单例类实例
	//MyCAS *p_a = MyCAS::GetInstance2(); //获取单例类对象，使用call_once()函数进行创建
}

int main() {
	/*
	MyCAS *p_a = MyCAS::GetInstance(); //获取单例类对象，最好在使用多线程之前加载单例类实例
	MyCAS *p_a2 = MyCAS::GetInstance(); //获取单例类对象，p_a2与p_a指向同一地址
	*/
	std::thread t1(getSingleClass);
	std::thread t2(getSingleClass);
	t1.join();
	t2.join();
	return 0;
}