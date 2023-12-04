bin/bench: main.cc deps/nanobench.h Makefile
	@mkdir -p $(@D)
	clang++ -O3 -march=native -mtune=native main.cc -I deps -o $@

deps/nanobench.h:
	@mkdir -p $(@D)
	curl -sLo $@ https://raw.githubusercontent.com/martinus/nanobench/v4.3.11/src/include/nanobench.h

