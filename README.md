# rwlock-vs-atomic

Benchmarking the performance of a rwlock against atomic naively.
There're many obvious parameters that has a direct effect on the outcome,
like turbo-boost, that we do not really pay attention to here. Still, I hope
this can be a reference on deciding when to use atomics against lock or rwlock.

## integer

A program that does read-write operations on either an atomic integer or an
integer via a read-write mutex.

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

### Read-heavy and work done in the locked area

It only makes sense to use rw-lock if certain amount of work is done in the
locked state. In this case, 1000 operations are done on the integer under
lock (or on the atomic itself).

Atomic takes longer in this case. `perf` could be used to track what is going
on, but probably high contention causes flushes and retries.

On a 11th Gen Intel(R) Core(TM) i7-11850H @ 2.50GHz:

```
make run-integer-read-heavy-busy-loop
./integer 19 1 1000 16 200000
Running with 16 threads.
std::shared_mutex took 1043 milliseconds.
pthread_rwlock took 1059 milliseconds.
std::mutex took 2499 milliseconds.
atomic took 3658 milliseconds.
race took 67 milliseconds.
```

If 100 operations are done:

```
./integer 19 1 100 16 2000000
Running with 16 threads.
std::shared_mutex took 8144 milliseconds.
pthread_rwlock took 7978 milliseconds.
std::mutex took 8226 milliseconds.
atomic took 4368 milliseconds.
race took 235 milliseconds.
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
