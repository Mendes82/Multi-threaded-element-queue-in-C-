#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <unistd.h>

template <typename T>
class Queue 
{
   private:
       std::queue<T> data;
       std::mutex mutex_;
       std::condition_variable cond_;
       const unsigned int max_size; 
       int Sleep;
       bool SleepFinished;
   public:
   	   Queue(int max_size)
   	        : max_size(max_size),Sleep(50),SleepFinished(false) //condition SleepFinished not used
			{}
       void push(T element) 
           {
             std::unique_lock<std::mutex> mlock(mutex_);
             while (data.size() >= max_size) 
             {
              std::cout << "blocks, waiting for pop" << std::endl;
              //std::this_thread::sleep_for(std::chrono::milliseconds(Sleep));  //optional
              cond_.wait(mlock);
              std::cout << "push (" << element <<") is realesed"  <<std::endl;
             }
             data.push(element);
             std::cout << "Writing, queue element: " << element << "   size: " <<  data.size() << std::endl;
             mlock.unlock();
             cond_.notify_one();
           }
       T pop() 
           {
             std::unique_lock<std::mutex> mlock(mutex_);
             while (data.empty()) 
             {
              std::cout << "blocks, waiting for push" << std::endl;
              //std::this_thread::sleep_for(std::chrono::milliseconds(Sleep));  //optional
              cond_.wait(mlock);
              std::cout << "pop is realesed "<< std::endl;
             }
             auto front = data.front();
             data.pop();
             std::cout << "Reading, popped element: " << front << "   size: " << data.size() << std::endl;
             mlock.unlock();
             cond_.notify_one();
             return front;
           }
	    int size() const //not used
			{
			 int result = data.size();
			 return result;
			}
 
};

void Write(Queue<int>& q) {
  for (int i = 1; i< 1001; ++i) 
  {
    q.push(i);
  }
}

void Read(Queue<int>& q, unsigned int id) {
  for (int i = 1; i< 1001; ++i) 
  {
    q.pop();
  }
}

int main() {
  Queue<int> q (2);

  using namespace std::placeholders;

  // Writing thread
  std::thread  Writing(std::bind(&Write, std::ref(q)));

  // Reading thread
  std::thread Reading(std::bind(&Read, std::ref(q), 1));
  
  Writing.join();
  Reading.join();

  std::cout << "test finished, loop exited" << std::endl;
 
  return 0;
}