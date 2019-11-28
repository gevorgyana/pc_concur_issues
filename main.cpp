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

  /**
   * Пока что остановился на двух вариантах.
   * 1 - не нужен никакой третий наблюдатель,
   * можно разрешать дедлок самостоятельно каждым из
   * рабочих потоков. - здесь только 2 потока, поэтому так можно,
   * можно ли развить это на много потоков?

   * +) можно просто сделать так, чтобы ожидать мог только один -
   * но это сильно ограничивает - тоже не вариант

   * 2 - надо захватить мутекс и "сфотографировать", что происходит
   * со счетчиками рабочих потоков. Можно оптимизировать - сделать так,
   * чтобы он триггерили проверяльщика, но 1) всe равно есть race condition,
   * 2) если нельзя вмешиваться в код рабочих потоков, то этот вариант не под-
   * ходит.

   * Остается такой вариант: сделать работу со счетчиками атомарной,
   * кешировать их значения в каждом потоке. (хотя
   * это тоже значит менять код потоков из классичекой задачи)
   * Но если так сделать, то можно по мутексу блокировать обновление
   * любого потка и проверять кондишен, который приведет к дедлоку.
   * проблема такого подхода - тоже можно сделать race condition -
   * если проверялтщик недостаточно часто будет работать, то это будет очнь вероятно.
   * например, он будет выполняться на одном ядре с остальными, и тогда в определенные
   * моменты просто не будет работать. взаимодействовать будут просто 2 остальных потока,
   * без проверяльщика.

   * => непонятно, как гарантированно найти способ наблюдать за дедлоком
   *
   * Придется менять код самих потоков, если надо гарантировать нахождение дедлока.
   * Можно было бы запретить спать двум одновременно. Но логика моего подхода в том,
   * что мы берем минимально возможный шаг, который мы можем контроллировать, и который
   * по идее **может** привести к дедлоку - и устраняем его. Просто не даем системе дальше
   * продвинуться, пока не произойдет остабление ситуации.

   * TODO - здесь тоже race condition!???? - да, но он не мешает
   * producer:
   *  { // works with data
   *    while (one_has_not_finished_sleeping) - race - если мы не успели сказать,что мы уже не спим,
   * в данном случае ничего не произойдет, рано или поздно другой поток это сделает, и мы продолжим
          {
            ;
          }

        теперь мы захватываем флаг, и делаем "спатки"
        когда закончили спать, освобождаем флаг.
        это называется StTRICT ALTERNATION - работает только для
        двух потоков. Если коротко, делать strict alternation по операции seep.
        Если больше потоков, нвдо делать critical section по sleep, либо
        решать задач с помощью семафоров, поскольку там не теряется сигнал.
   *  }

   * consumer:
   *  [same as above]

   * Итого: непонятно, что должно работать!!! Как полностью контроллировать
   * дедлок? Если только 2 потока, то +- можно это делать, хотя strict alternaion
   * не самый лучший варант - много времени будет на бизи-вейтинг - ну тут хотя бы
   * можно сделать yield

   * В общем случае: как наблюдать/разрешать - вообще неясно (это и для слуая их 2х потоков сложно
   * сделать), сторонний поток не может при каждом шаге заморозить всех остальных - хотя если сипользовать
   * yield, то может и может? TODO. Ну если 3+, то при условии, что а) мы можем заморозить состояние системы
   * и посмотреть, что с каждым счетчиком и б) что можно заставить каждый поток не выполняться, (например,
   * с помощью yield), пока не отработает проверяльщик - все нормально должно работать
   */

 private:
  std::condition_variable condvar_;
  std::mutex mutex_;
  bool consumer_sleep_request_,
    producer_sleep_request_;
  std::atomic<int> global_counter_value_{0};
  int consumer_snap_counter_{0},
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
        spdlog::info("producer incremented {} -> {}", prev, global_counter_value_);
      }

      // if needed, wake up the other one
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
        spdlog::info("consumer decremented {} -> {}", prev, global_counter_value_);
      }

      // if needed wake up the other one

      if (global_counter_value_ == N - 1)
      {
        condvar_.notify_one();
      }

    }
  }

  void Observe()
  {
    // TODO native handle cppref to terminate threads - unix only - compiler defined option

    return;
  }
};

int main()
{
  PCDeadlockSimulator simulator;
  simulator.Run();
  return 0;
}
