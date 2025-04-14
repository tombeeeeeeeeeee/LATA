#include "ThreadPool.h"

#include <iostream>

ThreadPool::ThreadPool(int threadCount) : shutdown(false)
{
	threads.reserve(threadCount);
	for (int i = 0; i < threadCount; i++) {
		threads.emplace_back(std::bind(&ThreadPool::ThreadEntry, this, i));
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock <std::mutex> l(lock);

		shutdown = true;
		conditionVariable.notify_all();
	}

	for (auto& thread : threads) {
		thread.join();
	}
}

void ThreadPool::DoJob(std::function<void(void)> func)
{
	std::unique_lock <std::mutex> l(lock);

	jobs.emplace(std::move(func));
	conditionVariable.notify_one();
	jobsWaiting++;
}

int ThreadPool::getJobsWaiting() const
{
	return jobsWaiting;
}

void ThreadPool::AddThread()
{
	threads.emplace_back(std::bind(&ThreadPool::ThreadEntry, this, threads.size()));
}

size_t ThreadPool::getThreadCount() const
{
	return threads.size();
}

void ThreadPool::ThreadEntry(int i)
{
	std::function <void(void)> job;

	while (true)
	{
		{
			std::unique_lock <std::mutex> l(lock);

			while (!shutdown && jobs.empty()) {
				conditionVariable.wait(l);
			}

			if (jobs.empty())
			{
				// Shutting down
				return;
			}

			job = std::move(jobs.front());
			jobs.pop();
		}

		job();
		std::unique_lock<std::mutex> l(lock);
		jobsWaiting--;
	}
}

