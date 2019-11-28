#include <bits/stdc++.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// the following code also has race conditions!!!

#define N 10

// TODO strict allternation!!!!! - even with strict alternation,
// this is useless as there may be no progress DONE

class S
{

 public:
  void R()
  {
    std::thread t1(&S::P, this);
    std::thread t2(&S::C, this);
    t1.join();
    t2.join();
  }

 private:

  std::atomic<int> gc_{0}; int cc_{0}, pc_{0};
  std::mutex mut_, c_mut_;
  std::condition_variable cv_;
  // alternation flag
  std::atomic<bool> producer_asleep_{false},
    consumer_asleep_{false};

  void P()
  {
    while (true)
    {

      // if needed to go sleep
      {
        std::unique_lock<std::mutex> l(mut_);
        if (gc_ == N)
        {
          while (consumer_asleep_)
          {
            spdlog::warn("producer yields");
            //std::this_thread::yield();
          }

          // context switch -> and consumer is now asleep!
          // this is a wrong solution DONE

          producer_asleep_ = true;
          spdlog::warn("producer is about to wait");
          cv_.wait(l);
          producer_asleep_ = false;
        }
      }


      // process shared data
      {
        std::unique_lock<std::mutex> l(c_mut_);
        int prev = gc_++;
        pc_ = gc_;
        spdlog::info("producer incremented {} -> {}", prev, gc_);
      }

      // if needed, wake up the other one
      if (gc_ == 1)
      {
        cv_.notify_one();
      }


    }
  }

  void C()
  {
    while (true)
    {

      // no data -> sleep
      {
        std::unique_lock <std::mutex>l(mut_);
        if (gc_ == 0)
        {
          while (producer_asleep_)
          {
            spdlog::warn("consumer about to yield");
            //std::this_thread::yield();
          }
          consumer_asleep_ = true;
          spdlog::warn("consumer is going to wait");
          cv_.wait(l);
          consumer_asleep_ = false;
        }
      }


      // process shared data
      {
        std::unique_lock<std::mutex> l(c_mut_);
        int prev = gc_--;
        cc_ = gc_;
        spdlog::info("consumer decremented {} -> {}", prev, gc_);
      }

      // if needed wake up the other one

      if (gc_ == N - 1)
      {
        cv_.notify_one();
      }

    }
  }


  std::atomic<int> shared_resource_;

};

int main()
{
  S s;
  s.R();
  return 0;
}
