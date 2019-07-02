#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <atomic>
#include <future>
//#include <condition_variable>
//#include <thread>
//#include <functional>
#include <stdexcept>

namespace std
{
	//�̳߳��������,Ӧ������Сһ��
#define  THREADPOOL_MAX_NUM 16
//#define  THREADPOOL_AUTO_GROW

//�̳߳�,�����ύ��κ�������ķ����ʽ����������ִ��,���Ի�ȡִ�з���ֵ
//��ֱ��֧�����Ա����, ֧���ྲ̬��Ա������ȫ�ֺ���,Opteron()������
	class threadpool
	{
		using Task = function<void()>;	//��������
		vector<thread> _pool;     //�̳߳�
		queue<Task> _tasks;            //�������
		mutex _lock;                   //ͬ��
		condition_variable _task_cv;   //��������
		atomic<bool> _run{ true };     //�̳߳��Ƿ�ִ��
		atomic<int>  _idlThrNum{ 0 };  //�����߳�����

	public:
		inline threadpool(unsigned short size = 4) { addThread(size); }
		inline ~threadpool()
		{
			_run = false;
			_task_cv.notify_all(); // ���������߳�ִ��
			for (thread& thread : _pool) {
				//thread.detach(); // ���̡߳���������
				if (thread.joinable())
					thread.join(); // �ȴ���������� ǰ�᣺�߳�һ����ִ����
			}
		}

	public:
		// �ύһ������
		// ����.get()��ȡ����ֵ��ȴ�����ִ����,��ȡ����ֵ
		// �����ַ�������ʵ�ֵ������Ա��
		// һ����ʹ��   bind�� .commit(std::bind(&Dog::sayHello, &dog));
		// һ������   mem_fn�� .commit(std::mem_fn(&Dog::sayHello), this)
		template<class F, class... Args>
		auto commit(F&& f, Args&&... args) ->future<decltype(f(args...))>
		{
			if (!_run)    // stoped ??
				throw runtime_error("commit on ThreadPool is stopped.");

			using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, ���� f �ķ���ֵ����
			auto task = make_shared<packaged_task<RetType()>>(
				bind(forward<F>(f), forward<Args>(args)...)
				); // �Ѻ�����ڼ�����,���(��)
			future<RetType> future = task->get_future();
			{    // ������񵽶���
				lock_guard<mutex> lock{ _lock };//�Ե�ǰ���������  lock_guard �� mutex �� stack ��װ�࣬�����ʱ�� lock()��������ʱ�� unlock()
				_tasks.emplace([task]() { // push(Task{...}) �ŵ����к���
					(*task)();
				});
			}
#ifdef THREADPOOL_AUTO_GROW
			if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
				addThread(1);
#endif // !THREADPOOL_AUTO_GROW
			_task_cv.notify_one(); // ����һ���߳�ִ��

			return future;
		}

		//�����߳�����
		int idlCount() { return _idlThrNum; }
		//�߳�����
		int thrCount() { return _pool.size(); }
#ifndef THREADPOOL_AUTO_GROW
	private:
#endif // !THREADPOOL_AUTO_GROW
		//���ָ���������߳�
		void addThread(unsigned short size)
		{
			for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
			{   //�����߳�����,�������� Ԥ�������� THREADPOOL_MAX_NUM
				_pool.emplace_back([this] { //�����̺߳���
					while (_run)
					{
						Task task; // ��ȡһ����ִ�е� task
						{
							// unique_lock ��� lock_guard �ĺô��ǣ�������ʱ unlock() �� lock()
							unique_lock<mutex> lock{ _lock };
							_task_cv.wait(lock, [this] {
								return !_run || !_tasks.empty();
							}); // wait ֱ���� task
							if (!_run && _tasks.empty())
								return;
							task = move(_tasks.front()); // ���Ƚ��ȳ��Ӷ���ȡһ�� task
							_tasks.pop();
						}
						_idlThrNum--;
						task();//ִ������
						_idlThrNum++;
					}
				});
				_idlThrNum++;
			}
		}
	};

}

#endif  //https://github.com/lzpong/
