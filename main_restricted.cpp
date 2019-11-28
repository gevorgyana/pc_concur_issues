#include <bits/stdc++.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#define N 10

// modify the code to enable strict alternation
// changes from the original problem:
// - need cached values
// - they do not have to be atomic? TODO

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

  void P()
  {
    while (true)
    {
      // if needed to go sleep
      if (gc_ == N)
      {
        std::unique_lock<std::mutex> l(mut_);
        cv_.wait(l);
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
      if (gc_ == 0)
      {
        std::unique_lock <std::mutex>l(mut_);
        cv_.wait(l);
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
