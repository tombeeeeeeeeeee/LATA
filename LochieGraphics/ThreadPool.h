#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>


class ThreadPool
{
private:
	std::mutex lock;
	std::condition_variable conditionVariable;
	bool shutdown;
	std::queue <std::function <void(void)>> jobs;
	std::vector <std::thread> threads;
	volatile int jobsWaiting = 0;

public:
	ThreadPool(int threads);
	~ThreadPool();

	void DoJob(std::function <void(void)> func);

	int getJobsWaiting() const;

private:
	void ThreadEntry(int i);
};

