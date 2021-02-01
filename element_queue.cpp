#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream> 
#include <unistd.h>
#include <functional>
#include <atomic>

template <class T> class Queue
{
	private:
		std::mutex mutex_;
		bool waitFinished;
		int max_size;
		int Sleep;
	public:
    	std::queue<T> data;
		Queue(int max_size = 2)
			: max_size(max_size),waitFinished(false),Sleep(50)
			{}
		
		void push(T element)
			{
				while (data.size() >= max_size)
				{
				 std::unique_lock<std::mutex> mlock(mutex_);
				 std::cout << "block, waiting for pop" << std::endl;
				 mlock.unlock();
				 std::this_thread::sleep_for(std::chrono::milliseconds(Sleep));
				 waitFinished = true;
				}
				std::unique_lock<std::mutex> mlock(mutex_);
				data.push(element);
                std::cout << "Write, queue element: " << element << " size: " << data.size() << std::endl;
				mlock.unlock();
               
			}
		T pop()
			{
				while (data.size() <= 0)
				{
				 std::unique_lock<std::mutex> mlock(mutex_);
				 std::cout << "block, waiting for push " << std::endl;
				 mlock.unlock();
				 std::this_thread::sleep_for(std::chrono::milliseconds(Sleep));
				 waitFinished = true;
				}
				std::unique_lock<std::mutex> mlock(mutex_);
				T element = data.front(); 
				data.pop();
                std::cout << "Read, front element: " << element << " size: " << data.size() << std::endl;
				mlock.unlock();
				return element;
			}
		int size() const
			{
			 int result = data.size();
			 return result;
			}
};

void Write(Queue<int>& q) 
{
  for (int i = 1; i< 100; ++i) 
  {
   q.push(i);
  }
}

void Read(Queue<int>& q) 
{
  for (int i = 1; i< 100; ++i) 
  {
    q.pop();
  }
}

int main() {
 Queue<int> q;

  using namespace std::placeholders;

  // Writing thread
  std::thread Writing(std::bind(&Write, std::ref(q)));

  // Reading thread
  std::thread Reading(std::bind(&Read, std::ref(q)));

  Writing.join();
  Reading.join();
 
 std::cout << "test finished, loop exited" << std::endl;

  return 0;
}
