#include <cstddef>
#include <cstdint>

#include <thread>
#include <mutex>
#include <condition_variable>

struct Semaphore {
  std::mutex m;
  std::condition_variable cv;

  std::size_t available;
};

void wait(Semaphore* s) {
  std::unique_lock<std::mutex> lock(s->m);
  s->cv.wait(lock, [=]{ return s->available > 0; });
  s->available--;
}

void signal(Semaphore* s) {
  std::unique_lock<std::mutex> lock(s->m);
  s->available++;
  s->cv.notify_one();
}

int main() {
  Semaphore s;

  wait(&s);
  signal(&s);
}
