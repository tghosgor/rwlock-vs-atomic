integer: integer.cpp
	$(CXX) $< -o $@ -O2 -std=c++17 -march=native -pthread -DNDEBUG

run-integer-read-heavy: integer
	./$< 19 1

run-integer-read-heavy-no-contention: integer
	./$< 19 1 1

run-integer-equal-readwrite: integer
	./$< 1 1

run-integer-no-write: integer
	./$< 1 0
