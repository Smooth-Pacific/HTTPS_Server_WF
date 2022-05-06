#ifndef WF_TSAFE_Q_H
#define WF_TSAFE_Q_H

#include<atomic>

#include<mutex>
#include<condition_variable>
#include<thread>
#include<queue>
#include<iostream>
#include<sstream>

#define SAMPLE 5

std::string logstring = "";
std::stringstream log(logstring);
std::mutex logex;
class task
{
	static int task_count;
	int task_id;
public:
	task() :task_id(++task_count)
	{
		std::lock_guard<std::mutex> logtask(logex);
		log << "d" << task_id;
	}
	task(const task& other) :task_id(other.task_id)
	{
		std::lock_guard<std::mutex> logtask(logex);
		log << "c" << task_id;
	}
	task(task&& other)
	{
		task_id = other.task_id;
		task_count++;
		other.task_id = task_count;
		std::lock_guard<std::mutex> logtask(logex);
		log << "m" << task_id;
	}
	task& operator =(const task& other)
	{
		if (&other == this)
		{
			return *this;
		}
		std::lock_guard<std::mutex> logtask(logex);
		log << "(" << other.task_id << "->" << task_id << ")";
		task_id = other.task_id;
		return *this;
	}
	task& operator =(task&& other)
	{
		if (&other == this)
		{
			return *this;
		}

		std::lock_guard<std::mutex> logtask(logex);
		log << "(" << other.task_id << "->" << task_id << ", ";
		task_id = other.task_id;
		task_count++;
		other.task_id = task_count;
		log << other.task_id << "->" << task_id << ")";
		return *this;
	}
	~task()
	{
		std::lock_guard<std::mutex> logtask(logex);	// putting a lock on a destructor is probably a really risky move, but I'm just messing around with the string stream anyway
		log << "~" << task_id;
	}

	void operator()()
	{
		waste_time();
	}

	void waste_time()
	{
		volatile int c = 5;
		for (int i = 0; i < 1000; i++)
		{
			if (c != 5)
			{
				break;
			}
		}
	}
};

int task::task_count = 0;


template <typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<T>data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue() {}
	threadsafe_queue(threadsafe_queue const& other)
	{
		std::lock_guard<std::mutex> lk(other.mut);
		data_queue = other.data_queue;
	}

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(new_value);
		data_cond.notify_one();
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		value = data_queue.front();
		data_queue.pup();
	}

	//end column on slide

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {return !data_queue.empty(); });
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return false;
		value = data_queue.front();
		data_queue.pop();
		return true;
	}
	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mut);
		if (data_queue.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
	int count() const
	{
		return data_queue.size();
	}
};


#endif
