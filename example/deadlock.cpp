#include <iostream>

#include <thread>
#include <mutex>

#include <chrono>
using namespace std::chrono_literals;


std::mutex s1{};
std::mutex s2{};

int main() {
  std::jthread t1([]() {
    std::cout << "[thread 1]: Lock `s1` - `s2`\n";
    s1.lock();
    std::this_thread::sleep_for(3s);
    s2.lock();

    std::cout << "[thread 1]: Unlock `s1` - `s2`\n";
    s1.unlock();
    s2.unlock();
  });

  std::jthread t2([]() {
    std::cout << "[thread 2]: Lock `s2` - `s1`\n";
    s2.lock();
    std::this_thread::sleep_for(3s);
    s1.lock();

    std::cout << "[thread 2]: Unlock `s2` - `s1`\n";
    s2.unlock();
    s1.unlock();
  });
}
