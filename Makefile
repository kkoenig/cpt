check: test bench

build: bin/bench bin/tests

test: bin/tests
	$<

bench: bin/bench
	$<

clean:
	rm -rf ./deps ./bin

bin/bench: bench/main.cc deps/nanobench.h Makefile cpt.h
	@mkdir -p $(@D)
	clang++ -O3 -march=native -mtune=native $< -I./deps -I. -o $@

bin/tests: tests/main.cc Makefile cpt.h deps/doctest.h
	@mkdir -p $(@D)
	clang++ -fsanitize=undefined -fsanitize=address $< -I./deps -I. -o $@

deps/doctest.h:
	@mkdir -p $(@D)
	curl -sLo $@ https://github.com/doctest/doctest/releases/download/v2.4.11/doctest.h

deps/nanobench.h:
	@mkdir -p $(@D)
	curl -sLo $@ https://raw.githubusercontent.com/martinus/nanobench/v4.3.11/src/include/nanobench.h

.PHONY: check build clean test bench
