#include <bits/stdc++.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
/**
 * Simulate producer-consumer item loss that leads to
 * deadlock; try to increase the probability of
 * a deadlock; verify and detect deadlock;
 * use thread-safe buffer to store products;
 * use sleep-wakeup primitives;

 * solutions:
 * - use condition variables to simulate sleep-wakeup
 * - deadlock can be caused by item loss, eventually,
 * - producer will sleep forever because consumer will
 * not state that they want to consume new items
 * - to increase propability of deadlock, artificially
 * yield when u read the variabe; in the original text by
 * Tanenbaum, u have to break the if (cnt == 0) ||| sleep();
 * so in the code, whis would become:
 * 1) check
 * 2) check successful? yield
 * 3) sleep
 *
 * deadlock verification mode:
 * Чтобы проверить, есть ли делок, достаточно
 * проверить условие: (проодюсер хранит  count N, consumer - count 0)
 *
 * TODO implement demonstration;
 * TODO benchmark this code; a python script to
 * automate this process and plot average values;
 */

// to see item loss, there have to be values!
// not just counter

#define N 10

/**
   try this and examine compiler messages
std::vector<std::thread> = {std::thread(***)}
 */

class PCDeadlockSimulator {
 public:
  void Run()
  {
    std::vector <std::thread> threads(3);
    threads[0] = std::thread(&PCDeadlockSimulator::ProduceSafe, this);
    threads[1] = std::thread(&PCDeadlockSimulator::ConsumeSafe, this);
    //threads[2] = std::thread(&PCDeadlockSimulator::Observe, this);

    for (auto& thread: threads)
    {
      thread.join();
    }
  }

 private:
  std::condition_variable condvar_;
  std::mutex mutex_;
  std::atomic_bool flag_{false};
  std::atomic<int> global_counter_value_{0};

  void ProduceSafe()
  {
    while (true)
    {
      // if needed to go sleep
      if (global_counter_value_ == N)
      {
        std::unique_lock<std::mutex> l(mutex_);
        condvar_.wait(l);
      }

      // process shared data
      int prev = global_counter_value_++;
      spdlog::info("producer incremented {} -> {}", prev, global_counter_value_);

      // if needed, wake up the pther one
      if (global_counter_value_)
      {
        condvar_.notify_one();
      }
    }
  }

  void ConsumeSafe()
  {
    while (true)
    {
      // no data -> sleep
      if (global_counter_value_ == 0)
      {
        std::unique_lock <std::mutex>l(mutex_);
        condvar_.wait(l);
      }

      // process shared data
      int prev = global_counter_value_--;
      spdlog::info("consumer decremented {} -> {}", prev, global_counter_value_);

      // if needed wake up the other one
      if (global_counter_value_ == N - 1)
      {
        condvar_.notify_one();
      }
    }
  }
};

int main()
{
  PCDeadlockSimulator simulator;
  simulator.Run();
  return 0;
}
