---
highlighter: "shiki"
lineNumbers: true

fonts:
  # Used for code blocks.
  mono: "Fira Mono"
  # Used for other texts.
  sans: "Playpen Sans"
  serif: "Playpen Sans"

class: "tracking-wide leading-relaxed tab-4"
---

<div class="text-center text-shadow-lg px-2 py-1">

# **Semaphore**

By `Cunny Operation` group

</div>


---
class: "tracking-wide leading-relaxed tab-4"
---

# **Table of contents**

<div class="h-full grid grid-rows-2 grid-cols-2 justify-evenly">
<span class="text-center">
  <h2>01</h2>
  <h2>What is a semaphore</h2>
</span>
<span class="text-center">
  <h2>02</h2>
  <h2><code>wait()</code> and <code>signal()</code> functions</h2>
</span>
<span class="text-center">
  <h2>03</h2>
  <h2>Binary and counting semaphore</h2>
</span>
<span class="text-center">
  <h2>04</h2>
  <h2>The disadvantages of semaphore</h2>
</span>
</div>


---
layout: "image-right"
image: "https://upload.wikimedia.org/wikipedia/commons/thumb/d/d9/Edsger_Wybe_Dijkstra.jpg/800px-Edsger_Wybe_Dijkstra.jpg"

class: "tracking-wide leading-relaxed tab-4"
---

## **01. What is a semaphore**

`Semaphore` là một kiểu dữ liệu trừu tượng dùng để kiểm soát thứ tự truy cập của những tiến trình đang chạy đồng thời.

Về cơ bản, `Semaphore` là một biến được chia sẻ và sử dụng giữa nhiều tiến trình khác nhau
nhằm tránh xảy ra lỗi trong đồng bộ hoá tiến trình trong môi trường đa xử lý.

<br>

<span class="text-xs italic">

Khái niệm về <code>Semapore</code> được nghiên cứu bởi nhà khoa học người Hà Lan Edsger W. Dijkstra
(Ông còn được biết đến với thuật toán tìm đường đi ngắn nhất trong đồ thị - [Dijkstra's algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm))

</span>


---

## **02. `wait()` and `signal()` functions**

Hay còn có tên gọi khác là:
* `P()` / `V()`
* `wait()` / `post()`
* `lock()` / `unlock()`
* `acquire()` / `release()`

Hàm `wait()` dùng để thông báo khi có tiến trình đang tiến vào [critical section](https://en.wikipedia.org/wiki/Critical_section)
và hàm `signal()` dùng để thông báo khi tiến trình đã thực hiện xong.

<div class="grid grid-cols-2 gap-4">
<div>

##### `wait()`
```c
void wait(Semaphore s) {
  while (s <= 0); // Busy waiting
  s--;
}
```

</div>
<div>

##### `signal()`
```c
void signal(Semaphore s) {
  s++;
}
```

</div>
</div>


---

## **02. `wait()` and `signal()` functions**

**Lưu ý**: Mọi sự thay đổi tới `Semaphore s` trong cả hai thao tác `wait()` và `signal()` đều phải là ***atomic operations***.

```c {all|1|3,6,7,11|all}
#include <stdatomic.h>

typedef atomic_int Semaphore;

void wait(Semaphore* s) {
  while (atomic_load(s) <= 0); // Busy waiting
  atomic_fetch_sub(s, 1);
}

void signal(Semaphore* s) {
  atomic_fetch_add(s, 1);
}
```

***Atomic operation*** đảm bảo khi một biến đang được truy cập, không tiến trình nào khác có thể làm ảnh hưởng đến nó.


---
layout: "iframe-right"

url: "https://en.cppreference.com/w/cpp/thread/counting_semaphore"
scale: 0.6
---

## **Binary and counting semaphore**

<br>

> Theo như C++ reference, bản chất của **binary semaphore** là **counting semaphore** với giá trị tối đa nhỏ nhất là `1`.
> ```cpp
> using binary_semaphore = std::counting_semaphore<1>;
> ```

<br>

##### `signal()`
```cpp
void signal(Semaphore* s, int update = 1) {
  atomic_fetch_add(s, update); // s += update;
}
```


---

# **Binary semaphore**

**Binary semaphore** có thể được dùng thay thế cho **mutex** để thông báo khi một tiến trình bắt đầu tiến vào và ra khỏi critical section.

```cpp {all|4-6|6,10|9|12-28|15-17|30-40|36-38|14-18|14,18|35-39|35,39|18,26-27|all} {maxHeight:'370px'}
#include <iostream>
#include <chrono>

// Dùng để tạo tiến trình con (thread), semaphore
#include <thread>
#include <semaphore>


bool is_thread_running = true;
std::binary_semaphore g_guard{1};

void thread_runner() {
  while (true) {
    g_guard.acquire();
    {
      if (!is_thread_running) break;
    }
    g_guard.release();

    std::cout << "Hello\n";

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(140ms);
  }

  // Khi thoát khỏi vòng lặp thì dòng `g_guard.release()` trong vòng lặp sẽ không được chạy.
  g_guard.release();
}

int main() {
  std::jthread worker(thread_runner);

  std::getchar();

  g_guard.acquire();
  {
    is_thread_running = false;
  }
  g_guard.release();
}
```


---

<Link to="6" class="no-underline">CppRef</Link>

```sh {lines:false}
$ g++ --version
g++ (GCC) 13.2.0
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# `<semaphore>` chỉ có trong C++20, nên cần thêm `-std=c++20` vào.
$ g++ -std=c++20 -Wall -Wextra demo-binary-semaphore.cxx -o demo-binary-semaphore.exe
```

<br>

```sh {lines:false}
$ ./demo-binary-semaphore.exe
Hello
Hello
Hello
Hello
Hello
Hello
Hello
Hello
Hello

Hello
```


---

## **Sự khác biệt giữa *binary semaphore* và *mutex***

<br>

<div class="grid grid-cols-2 gap-4">
<div>

#### Binary semaphore:
* Signalling mechanism.
* Các tiến trình khác có thể mở khoá mà không cần phải do tiến trình đã khoá mở khoá.

</div>
<div>

#### Mutex:
* Locking mechanism.
* Tiến trình nào đã khoá thì phải do chính tiến trình đó mở khoá.

</div>
</div>


---

## **Counting semaphore**

Ngoài những chức năng tương tự như **binary semaphore**, **counting semaphore** có thể dùng để theo dõi số lượng tiến trình được phép truy cập vào critical section.


##### Producer - Consumer problem
```cpp {all} {maxHeight:'330px'}
std::counting_semaphore<N> sem_empty{N};
std::counting_semaphore<N> sem_full{0};

std::mutex buffer_guard;

void produce() {
  while (true) {
    Product product = make_product();

    sem_empty.acquire();
    {
      std::lock_guard<std::mutex> g(buffer_guard);
      add_product_to_buffer(product);
    }
    sem_full.release();
  }
}

void consume() {
  while (true) {
    Product product;

    sem_full.acquire();
    {
      std::lock_guard<std::mutex> g(buffer_guard);
      product = take_product_from_buffer();
    }
    sem_empty.release();

    use_product(product);
  }
}
```


---

## The disadvantages of semaphore

<br>

#### 01. Busy waiting

```c {2}
void wait(Semaphore s) {
  while (s <= 0); // Busy waiting
  s--;
}
```

Gây tốn tài nguyên CPU

<br>

<v-click>

<div class="grid grid-cols-2 gap-4">
<div>

```cpp {lines:false}
typedef atomic_int Semaphore;
```

</div>
<div>

```cpp {lines:false}
typedef struct Semaphore {
  std::mutex m;
  std::condition_variable cv;

  std::size_t available;
} Semaphore;
```

</div>
</div>

</v-click>


---

## Busy waiting

<br>

```cpp
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
```

Sử dụng **condition variable** để thông báo rằng tiến trình này sẽ bắt đầu đợi,
điều đó sẽ khiến cho hệ điều hành tự xử lý và sắp xếp cho các tiến trình khác chạy thay vì dành CPU ra chỉ để chạy `while (s <= 0);`.
