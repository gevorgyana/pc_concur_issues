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

/**
 * Если все будут пользоваться одним мутексом, то
 * когда произойдет дедлок, никто его не освободит.
 * Тогда и сам наблюдатель тоже будет наблокирован
 * Решение: наблюдатель не может пользоваться мутексом.
 * Пускай просто смотрит на атомарные переменные-кеши
 * (они должны быть атомарные, чтобы наблюдатель мог
 * за ними наблюдать); как только наблдатель словил
 * неправильное состояние, он выводит сообщение

 * проблема: наблюдатель делает false positive
 */

class PCDeadlockSimulator {
 public:
  void Run()
  {
    std::vector <std::thread> threads(3);
    threads[0] = std::thread(&PCDeadlockSimulator::ProduceSafe, this);
    threads[1] = std::thread(&PCDeadlockSimulator::ConsumeSafe, this);
    threads[2] = std::thread(&PCDeadlockSimulator::Observe, this);

    for (auto& thread: threads)
    {
      thread.join();
    }
  }

 private:
  std::condition_variable condvar_;
  std::mutex mutex_;
  bool consumed_,
    produced_;
  std::atomic<int> global_counter_value_{0},
    consumer_snap_counter_{0},
    producer_snap_counter_{0};
  std::mutex counter_access_mutex_;

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
      {
        std::unique_lock<std::mutex> l(counter_access_mutex_);
        int prev = global_counter_value_++;
        producer_snap_counter_ = global_counter_value_;
        //produced_ |= true;
        spdlog::info("producer incremented {} -> {}", prev, global_counter_value_);
      }

      // if needed, wake up the other one

      std::unique_lock<std::mutex> l(counter_access_mutex_);
      if (global_counter_value_ == 1)
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

      std::unique_lock<std::mutex> l(counter_access_mutex_);
      if (global_counter_value_ == 0)
      {
        std::unique_lock <std::mutex>l(mutex_);
        condvar_.wait(l);
      }


      // process shared data
      {
        std::unique_lock<std::mutex> l(counter_access_mutex_);
        int prev = global_counter_value_--;
        consumer_snap_counter_ = global_counter_value_;
        //consumed_ |= true;
        spdlog::info("consumer decremented {} -> {}", prev, global_counter_value_);
      }

      // if needed wake up the other one

      std::unique_lock<std::mutex> l(counter_access_mutex_);
      if (global_counter_value_ == N - 1)
      {
        condvar_.notify_one();
      }

    }
  }

  void Observe()
  {
    // TODO native handle cppref to terminate threads - unix only - compiler defined option
    while (true)
    {

      // проблема: как сделать так, чтобы одновременно получить доступ к двум
      // переменным. вероятно, проблема в этом, и не нужны флаги.
      if ((producer_snap_counter_ != N || consumer_snap_counter_ != 0) /*||
          !(produced_ && consumed_)*/)
      {
        spdlog::info("blank shot");
        // small optimization
        std::this_thread::yield();
        continue;
      }

      else
        break;

    }

    spdlog::critical("deadlock detected");
    return;
  }
};

int main()
{
  PCDeadlockSimulator simulator;
  simulator.Run();
  return 0;
}
