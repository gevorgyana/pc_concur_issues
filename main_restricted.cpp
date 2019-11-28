#include <bits/stdc++.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#define N 10

// TODO modify the code to enable strict alternation

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
  std::atomic<bool> af_{false};

  void P()
  {
    while (true)
    {
      af_ = false;

      // if needed to go sleep
      {
        std::unique_lock<std::mutex> l(mut_);
        if (gc_ == N)
        {
          spdlog::warn("entered critival region cv producer");

          while (af_)
          {
            using namespace std::literals::chrono_literals;
            std::this_thread::sleep_for(2s);
            spdlog::warn("Pxc");
            std::this_thread::yield();
          }
          af_ = true;
          cv_.wait(l);
          af_ = false;
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
      af_ = false;

      {
        std::unique_lock <std::mutex>l(mut_);
        if (gc_ == 0)
        {
          spdlog::warn("entered critival region cv consumer");
          while (af_)
          {
            using namespace std::literals::chrono_literals;
            std::this_thread::sleep_for(2s);
            spdlog::warn("C");
            std::this_thread::yield();
          }
          af_ = true;
          cv_.wait(l);
          af_ = false;
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
