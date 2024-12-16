#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;

void Daemon()
{
  while (1)
  {
    cout << "This is daemon thread, no one can kill it unless terminated" << endl;
    std::this_thread::sleep_for(1000ms);
  }
}

int main()
{
  std::thread t1(Daemon); // creating a thread (i.e. Daemon) on the main thread

  // now, either t1 can wait for the thread within Daemon to complete or detach (meaning it will detach from main thread i.e. t1) so as detached t1 thead won't wait for this thread meaning t1 thread will go ahead and do its job
  t1.detach(); // running in the background

  // now, telling this main thread to sleep for 5ms so to be able to view the messages from Daemon getting printed (Daemon has 1ms interval so it will print at least 5 times)
  std::this_thread::sleep_for(5000ms);

  /**
   * so, as `main function` invoked
   * 1. so, it will execute its first line code i.e.  std::thread t1(Daemon); -> this will spawn an another thread i.e. Daemon and next line
   * 2. is that thread i.e. t1.detach() will run in the background
   *
   * The moment, the main has finished waiting for 5ms -> it will hit the next line i.e. return 0 ->
   * system will see one more thread is running which is associated with the `Daemon method` so I'll have to kill this thread so Operating System * is actually taking care of killing this thread you don't have to anything with this
   *
   * but there are so many questions regarding this like but
   * what if I want to handle this thread I mean killing of this thread can be handled from anywhere it is possible you may have to create a
   * global variable or mutex or something like that so that you can modify that variable in main thread or somewhere else and `Daemon thread is * actually keeping track of that variable if it is tunned on then I'll have to kill it myself or complete my job or something like that
   * otherwise there is no direct communication from here to this thread (main -> std::this_thread::sleep_for(5000ms) -> std::this_thread::sleep_for(1000ms)) or we can implement * signal handler all those things will come in the future if you ask for so this is for the introduction of `Daemon threads`
   *
   *
   */

  return 0;
}

// https://www.youtube.com/watch?v=FRBHvbu7q4E&list=PLk6CEY9XxSICcxsEeT3AKAda1cAoQXhGv&index=5
// https://www.youtube.com/watch?v=gBlrKfaBp2s&list=PLXV2t2tSoKWjEKVpEc0SuiBLygCBZbYfx&index=1