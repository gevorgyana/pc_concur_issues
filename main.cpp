#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>

using namespace std::literals::chrono_literals;

// number of items in the queue
#define N 10

class PCDeadlockSimulator {
 public:

  PCDeadlockSimulator() :
      threads_(3)
  {

  }

  void Run()
  {
    threads_[0] = std::thread(&PCDeadlockSimulator::Produce, this);
    threads_[1] = std::thread(&PCDeadlockSimulator::Consume, this);
    threads_[2] = std::thread(&PCDeadlockSimulator::Observe, this);

    for (auto& thread: threads_)
    {
      thread.join();
    }
  }

 private:
  std::vector<std::thread> threads_;

  std::condition_variable sleep_wakeup_condvar_;
  std::mutex sleep_wakeup_mutex_;

  int global_counter_value_{0};
  int producer_counter_cached{0};
  int consumer_counter_cached{0};

  std::mutex data_protection_mutex_;

  void Produce()
  {
    while (true)
    {
      if (global_counter_value_ == N)
      {
        std::unique_lock<std::mutex> lock_guard(sleep_wakeup_mutex_);
        spdlog::warn("P going to sleep");
        sleep_wakeup_condvar_.wait(lock_guard);
      }

      {
        std::unique_lock<std::mutex> lock_guard(data_protection_mutex_);
        int previous_counter_value = global_counter_value_++;
        spdlog::info("producer incremented {} -> {}",
                     previous_counter_value,
                     global_counter_value_);

        producer_counter_cached = global_counter_value_;
      }

      if (global_counter_value_ == 1)
      {
        sleep_wakeup_condvar_.notify_one();
      }
    }
  }

  void Consume()
  {
    while (true)
    {
      if (global_counter_value_ == 0)
      {
        spdlog::warn("C going to sleep");
        std::unique_lock<std::mutex> lock_guard(sleep_wakeup_mutex_);
        sleep_wakeup_condvar_.wait(lock_guard);
      }

      {
        std::unique_lock<std::mutex> lock_guard(data_protection_mutex_);
        int previous_counter_value = global_counter_value_--;
        spdlog::info("consumer decremented {} -> {}",
                     previous_counter_value,
                     global_counter_value_);

        consumer_counter_cached = global_counter_value_;
      }

      if (global_counter_value_ == N - 1)
      {
        sleep_wakeup_condvar_.notify_one();
      }
    }
  }

  // this gives false positives ???
  void Observe()
  {
    while (true)
    {
      std::this_thread::sleep_for(1s);
      spdlog::warn("P: {}, C: {}", producer_counter_cached, consumer_counter_cached);
      if (consumer_counter_cached == 0 && producer_counter_cached == N)
      {
        spdlog::critical("Deadlock detected...");
        // https://gcc.gnu.org/ml/gcc-help/2015-08/msg00040.html
        // pthread_cancel does not behave very well, if we call it on
        // the native handle of a c++ thread -> leave the threads hanging
        break;
      }
    }
    return;
  }
};

int main()
{
  PCDeadlockSimulator simulator;
  simulator.Run();
  return 0;
}
