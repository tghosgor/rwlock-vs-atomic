// SPDX-License-Identifier: MIT

#include <pthread.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <thread>
#include <vector>

using hrc = std::chrono::high_resolution_clock;
using namespace std::literals::chrono_literals;

namespace {

std::size_t num_threads = std::thread::hardware_concurrency();
std::size_t read_ops;
std::size_t total_ops;
std::size_t num_ops = 10'000'000;

struct {
  std::shared_mutex mutex;
  int padding[64];
  std::uint64_t value = 0;
  int padding2[64];
} std_mutex_integer;

struct {
  pthread_rwlock_t mutex = PTHREAD_RWLOCK_INITIALIZER;
  int padding[64];
  std::uint64_t value = 0;
  int padding2[64];
} pthread_mutex_integer;

struct {
  int padding[64];
  std::atomic<std::uint64_t> value{0};
  int padding2[64];
} atomic_integer;;

struct {
  int padding[64];
  std::uint64_t value = 0;
  int padding2[64];
} integer;

std::uint64_t atomic_op() noexcept {
  std::random_device rd;
  std::mt19937 rng{rd()};
  volatile std::uint64_t v;

  for (std::size_t i = 0; i < num_ops; ++i) {
    if ((rng() % total_ops) < read_ops) {
      v = atomic_integer.value.load(std::memory_order_relaxed);
    } else {
      v = atomic_integer.value.fetch_add(1, std::memory_order_relaxed);
    }
  }

  return v;
}

std::uint64_t std_rwlock_op() noexcept {
  std::random_device rd;
  std::mt19937 rng{rd()};
  volatile std::uint64_t v;

  for (std::size_t i = 0; i < num_ops; ++i) {
    if ((rng() % total_ops) < read_ops) {
      std::shared_lock lock{std_mutex_integer.mutex};
      v = std_mutex_integer.value;
    } else {
      std::unique_lock lock{std_mutex_integer.mutex};
      v = std_mutex_integer.value += 1;
    }
  }

  return v;
}

std::uint64_t pthread_rwlock_op() noexcept {
  std::random_device rd;
  std::mt19937 rng{rd()};
  volatile std::uint64_t v;

  for (std::size_t i = 0; i < num_ops; ++i) {
    if ((rng() % total_ops) < read_ops) {
      pthread_rwlock_rdlock(&pthread_mutex_integer.mutex);
      v = std_mutex_integer.value;
      pthread_rwlock_unlock(&pthread_mutex_integer.mutex);
    } else {
      pthread_rwlock_wrlock(&pthread_mutex_integer.mutex);
      v = std_mutex_integer.value += 1;
      pthread_rwlock_unlock(&pthread_mutex_integer.mutex);
    }
  }

  return v;
}

std::uint64_t race_op() noexcept {
  std::random_device rd;
  std::mt19937 rng{rd()};
  volatile std::uint64_t v;

  for (std::size_t i = 0; i < num_ops; ++i) {
    if ((rng() % total_ops) < read_ops) {
      v = integer.value;
    } else {
      v = integer.value += 1;
    }
  }

  return v;
}

template <class F>
void run(const std::string& type, F&& f) {
  std::vector<std::thread> threads;

  const auto& start = hrc::now();

  for (std::size_t i = 0; i < num_threads; ++i) {
    threads.emplace_back(std::forward<F>(f));
  }

  for (auto& t : threads) t.join();

  const auto& duration = hrc::now() - start;

  std::cout << type << " took "
    << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
    << " milliseconds." << std::endl;
}

}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr <<
      "Usage: integer <read-ops> <write-ops> "
      "[<n-threads>=" << num_threads << "] [<num-ops>=" << num_ops << "]\n"
      "\tI.e. './integer 10 9 1 1000' will have a 9/10 chance "
      "to do a read and 1/10 chance to do a write, running 1000 operations in "
      "10 threads." << std::endl;

    return EXIT_FAILURE;
  }

  try {
    read_ops = std::stoull(argv[1]);
  } catch (const std::exception& e) {
    std::cerr << "Failed to parse read-ops: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  try {
    total_ops = read_ops + std::stoull(argv[2]);
  } catch (const std::exception& e) {
    std::cerr << "Failed to parse write-ops: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  if (argc > 3) {
    try {
      num_threads = std::stoull(argv[3]);
    } catch (const std::exception& e) {
      std::cerr << "Failed to parse n-threads: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (argc > 4) {
    try {
      num_ops = read_ops + std::stoull(argv[4]);
    } catch (const std::exception& e) {
      std::cerr << "Failed to parse num-ops: " << e.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::cout << "Running with " << num_threads << " threads." << std::endl;

  run("std::shared_mutex", std_rwlock_op);

  // some naive attempt at cooling down
  std::this_thread::sleep_for(5s);

  run("pthread_rwlock", pthread_rwlock_op);

  // some naive attempt at cooling down
  std::this_thread::sleep_for(5s);

  run("atomic", atomic_op);

  // some naive attempt at cooling down
  std::this_thread::sleep_for(5s);

  run("race", race_op);

  return EXIT_SUCCESS;
}
