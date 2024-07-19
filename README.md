# rwlock-vs-atomic

Benchmarking the performance of a rwlock against atomic naively.
There're many obvious parameters that has a direct effect on the outcome, like turbo-boost, that we do not really pay attention to here.

## integer

A program that does read-write operations on either an atomic integer or an integer via a read-write mutex.

### Read-heavy

5% chance to write.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-read-heavy
./integer 19 1
Running with 16 threads.
std::shared_mutex took 30209 milliseconds.
pthread_rwlock took 30493 milliseconds.
std::mutex took 15421 milliseconds.
atomic took 560 milliseconds.
race took 446 milliseconds.
```

### Equally distributed read-write

50% chance to write.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-equal-readwrite
./integer 1 1
Running with 16 threads.
std::shared_mutex took 30695 milliseconds.
pthread_rwlock took 32658 milliseconds.
std::mutex took 21552 milliseconds.
atomic took 1854 milliseconds.
race took 1025 milliseconds.
```

### No writes at all

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-no-write
./integer 1 0
Running with 16 threads.
std::shared_mutex took 14215 milliseconds.
pthread_rwlock took 13299 milliseconds.
std::mutex took 13454 milliseconds.
atomic took 69 milliseconds.
race took 73 milliseconds.
```

### Read-heavy with no contention

5% chance to write.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-read-heavy-no-contention
./integer 19 1 1
Running with 1 threads.
std::shared_mutex took 190 milliseconds.
pthread_rwlock took 173 milliseconds.
std::mutex took 143 milliseconds.
atomic took 34 milliseconds.
race took 35 milliseconds.
```
