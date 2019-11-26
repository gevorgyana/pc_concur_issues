#include <bits/stdc++.h>

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
 * it is possible to run he pogram in
 * deadlock verification mode:
 * this is not enough, but still must hold
 * when a deadlock is present
 * do the following; when a thread enters critical region,
 * mark that we are now using the resource (buffer as a whole);
 * when we exit, we remove the flag;
 * when the other process exits, they too print the flag;
 * sumup:
 * every process does a print satement; when they hang, we see it (? it it okay?)
 * for formal verification we need a third party, observer()
 * observer will look and see the flags and watch out for
 * a case when a cycle appears;

 * example:
 * A sets request info - to a special store
 * B sets request info - to a special store
 * observer runs periodically and checks buffers,
 * when it sees the sign of deadlock, it reports
 * "DEADLOCK HAS BEEN ENCOUNTERED"

 * bullshi ^
 *         \

 * Чтобы проверить, есть ли делок, достаточно в этой задаче
 * проверить условие: (проодюсер хранит  count N, consumer - count 0)
 *
 *
 * TODO use thread-safe circular buffer? is it good in this case?
 * TODO implement demonstrational test;
 * TODO benchmark this code; come up with a python script to
 * automate this process and plot average values;
 */

// Идейно мы расслоили присваивания на 2 слоя - глобальный и специфичный для
// потока

#define N 10
int count = 0;

class PCDeadlockSimulator {
 public:
  void Run()
  {

  }

 private:
  void Produce()
  {
    while (true)
    {
      producer_snapshot_counter_value_ = global_counter_value;
      if (producer_snapshot_counter_value_ == N)
      {
        // cv.wait()
      }
      ++global_counter_value;
      producer_snapshot_counter_value = global_counter_value;
      if (producer_snapshow_counter)
    }
  }

  void Consume()
  {

  }

  void Observe()
  {
    while (true)
    {
      if (producer_current_value_ == N && consumer_current_value_ == 0)
      {
        std::cout << "Deadlock detected. Shutting down..." << std::endl;
      }
    }
  }

  int producer_snapshot_counter_value_;
  int consumer_snapshot_counter_value_;
};

void tst1_app_runs() {
  PCDeadlockSimulator simulator;

}

int main() {

}
