// myThreadPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "threadpool.h"
#include <iostream>

using namespace std;

void fun1(int slp)
{
	printf("线程id为：  %d\n", std::this_thread::get_id());
	if (slp > 0) {
		printf(" ======= fun1 sleep %d  ========= \n", slp);
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
		//Sleep(slp );
	}
}
struct gfun {
	int operator()(int n) {
		printf("线程id为：  %d\n", std::this_thread::get_id());
		return 42;
	}
};

class A {
public:
	void cal(int num) {
		cout << "我是A的成员函数cal,传入参数为"<< num << endl;
	}
	static int ca2(int num) {
		cout << "我是A的成员函数ca2,传入参数为" << num << endl;
		return 110;
	}
};

int main() {
	A a;
	std::threadpool threadpool(2);
	std::future<int> gful1 = threadpool.commit(gfun{}, 100);
	std::future<int> gful2 = threadpool.commit(gfun{}, 200);
	std::future<int> gful3 = threadpool.commit(gfun{}, 300);
	std::future<int> gful4 = threadpool.commit(gfun{}, 400);
	std::future<int> gful5 = threadpool.commit(gfun{}, 500);
	std::future<void> ful1 = threadpool.commit(fun1, 600);
	std::future<void> ful2 = threadpool.commit(fun1, 700);
	std::future<void> ful3 = threadpool.commit(fun1, 800);
	std::future<void> ful4 = threadpool.commit(fun1, 900);
	std::future<void> ful5 = threadpool.commit(fun1, 1000);

	std::future<int> cful1 = threadpool.commit(&A::ca2, 100);

	cout <<"我是ca2的返回值：" << cful1.get() <<endl;
	return 0;
}

