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
 * deadlock verification: TODO
 *
 * TODO use thread-safe circular buffer? is it good in this case?
 * TODO implement demonstrational test;
 * TODO benchmark this code; come up with a python script to
 * automate this process and plot average values;
 */

class PCDeadlockSimulator {
  void produce() {

  }

  void consume() {

  }
};

void tst1_app_runs() {
  PCDeadlockSimulator simulator;

}

int main() {

}
