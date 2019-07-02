
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

// TODO: 添加要在此处预编译的标头
#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <functional>

namespace std 
{
	const int THREAD_MAX_NUM = 10;//线程池最大容量

	//线程池,可以提交变参函数或lambda表达式的匿名函数执行,可以获取执行返回值
	//不直接支持类成员函数, 支持类静态成员函数或全局函数,Opteron()函数等
	class threadpool
	{
		using Task = function<void()>;//定义可提交任务的参数类型
		vector<std::thread> _pool; //存放线程的线程池
		queue<Task> _tasks;   //存放任务的队列
		std::mutex _mutex;   //互斥量
		std::condition_variable _cv;  //条件变量
		atomic<bool> _run = true;  //线程池是否运行
		atomic<int> _idleThreadNum = 0;  //空闲线程数量
	public:
		inline threadpool(unsigned short size = 4) { addThread(size); }
		inline ~threadpool()
		{
			_run = false;
			_cv.notify_all();//通知所有线程
			for (std::thread& aThread : _pool)
			{
				if (aThread.joinable())  //如果线程可以join
				{
					aThread.join();//等待线程执行完毕
				}
			}
		}//end ~threadpool
	public:
		//使用模板函数处理线程入口函数及其传递的参数
		template <class F, class... Args>
		//future对象返回类型为函数f的返回值类型
		auto commit(F&& f, Args&&... args)->std::future<decltype(f(args...))>
		{
			//线程池停止运行
			if (!_run)
				throw runtime_error("Commit on ThreadPool is stopped");

			using RetType = decltype(f(args...));//函数f的返回值类型
			//打包成智能指针指向的可调用对象
			auto task = make_shared<packaged_task<RetType()>>(
				//使用完美转发保证参数的左右值属性不变
				//绑定线程入口函数和参数
				bind(std::forward<F>(f), std::forward<Args>(args)...)
			);
			std::future<RetType> ful = task->get_future();
			{
				std::lock_guard<std::mutex> lg(_mutex);//互斥量加锁
				//将任务添加到队列
				_tasks.emplace([task]() {
					(*task)(); //lambda函数，加函数指针
				});
			}//互斥量自动解锁
#ifdef  THREADPOOL_AUTO_GROW
			if (_idleThreadNum < 1 && _pool.size() < THREADPOOL_AUTO_GROW)
				addThread(1);
#endif //  THREADPOOL_AUTO_GROW

			_cv.notify_one();//唤醒一个线程执行任务
			return ful;
		}
		int idleThreadCount() { return _idleThreadNum; }
		int threadCount() { return _pool.size(); }
#ifndef THREADPOOL_AUTO_GROW
	private:
#endif // !THREADPOOL_AUTO_GROW
		//添加指定数量的线程
		void addThread(unsigned int size)
		{
			for (; _pool.size() < THREAD_MAX_NUM && size > 0; size--)
			{
				_pool.emplace_back([this]() {
					while (_run)
					{
						//获取一个
						Task task;
						{
							//加锁
							std::unique_lock<std::mutex> ul(_mutex);
							_cv.wait(ul, [this]() {
								return !_run || !_tasks.empty();
							});
							if (!_run && _tasks.empty())
								return;
							task = std::move(_tasks.front());
							_tasks.pop();
						}
						_idleThreadNum--;
						task();//执行任务
						_idleThreadNum++;
					}
				});
				_idleThreadNum++;
			}
		}
	};
}
#endif //THREAD_POOL_H
