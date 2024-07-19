integer: integer.cpp
	$(CXX) $< -o $@ -O2 -std=c++17 -march=native -pthread -DNDEBUG

.PHONY: run-integer-read-heavy
run-integer-read-heavy: integer
	./$< 19 1

.PHONY: run-integer-read-heavy-no-contention
run-integer-read-heavy-no-contention: integer
	./$< 19 1 1

.PHONY: run-integer-equal-readwrite
run-integer-equal-readwrite: integer
	./$< 1 1

.PHONY: run-integer-no-write
run-integer-no-write: integer
	./$< 1 0
