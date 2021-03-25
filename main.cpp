#include <algorithm>
#include <future>
#include <fstream>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include "Timer.hpp"

class Threads_Guard
{
public:

	explicit Threads_Guard(std::vector < std::thread >& threads) :
		m_threads(threads)
	{}

	Threads_Guard(Threads_Guard const&) = delete;

	Threads_Guard& operator=(Threads_Guard const&) = delete;

	~Threads_Guard() noexcept
	{
		try
		{
			for (std::size_t i = 0; i < m_threads.size(); ++i)
			{
				if (m_threads[i].joinable())
				{
					m_threads[i].join();
				}
			}
		}
		catch (...)
		{
			// std::abort();
		}
	}

private:

	std::vector < std::thread >& m_threads;
};

template < typename Iterator, typename T >
struct accumulate_block
{
	T operator()(Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}
};

template < typename Iterator, typename T >
T parallel_accumulate(Iterator first, Iterator last, T init, std::size_t number_of_threads)
{
	const std::size_t length = std::distance(first, last);

	if (!length)
		return init;

	const std::size_t number_threads = number_of_threads;
	const std::size_t block_size = length / number_threads;

	std::vector < std::future < T > > futures(number_threads - 1);
	std::vector < std::thread >		  threads(number_threads - 1);

	Threads_Guard guard(threads);

	Iterator block_start = first;

	for (std::size_t i = 0; i < (number_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		std::packaged_task < T(Iterator, Iterator) > task
		{
			accumulate_block < Iterator, T >() };

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), block_start, block_end);

		block_start = block_end;
	}

	T last_result = accumulate_block < Iterator, T >()(block_start, last);

	T result = init;

	for (std::size_t i = 0; i < (number_threads - 1); ++i)
	{
		result += futures[i].get();
	}

	result += last_result;

	return result;
}

int main(int argc, char** argv)
{
	std::vector< int > v(10000000);

	std::iota(v.begin(), v.end(), 1);

	std::size_t N = 10000;
	std::ofstream fout("output.txt");
	fout << "number_of_threads,time" << std::endl;

	for (auto i = 1; i < N; ++i)
	{
		Timer t;
		parallel_accumulate(v.begin(), v.end(), 0, i);
		t.stop();
		fout << i << "," << t.math() << std::endl;
	}

	system("pause");
	return EXIT_SUCCESS;
}