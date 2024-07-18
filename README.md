# rwlock-vs-atomic

Benchmarking the performance of a rwlock against atomic naively.
There're many obvious parameters that has a direct effect on the outcome, like turbo-boost, that we do not really pay attention to here.

## integer

A program that does read-write operations on either an atomic integer or an integer via a read-write mutex.

### Read-heavy run

5% chance to write.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-read-heavy
./integer 19 1
Running with 16 threads.
std::shared_mutex took 30262 milliseconds.
pthread_rwlock took 30407 milliseconds.
atomic took 541 milliseconds.
race took 422 milliseconds.
```

### An equally distributed read-write run

50% chance to write.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-equal-readwrite
./integer 1 1
Running with 16 threads.
std::shared_mutex took 29791 milliseconds.
pthread_rwlock took 29839 milliseconds.
atomic took 1756 milliseconds.
race took 883 milliseconds.
```

### No writes at all

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-no-write
./integer 1 0
Running with 16 threads.
std::shared_mutex took 14322 milliseconds.
pthread_rwlock took 13907 milliseconds.
atomic took 72 milliseconds.
race took 64 milliseconds.
```

### Read-heavy run with no contention

5% chance to write.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-read-heavy-no-contention
./integer 19 1 1
Running with 1 threads.
std::shared_mutex took 179 milliseconds.
pthread_rwlock took 192 milliseconds.
atomic took 38 milliseconds.
race took 32 milliseconds.
```
