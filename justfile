set shell := ["bash", "-cu"]

clang_format := env_var_or_default("CLANG_FORMAT", "clang-format-20")
cc := env_var_or_default("CC", "cc")
std_flags := "-std=c2x"
warn_flags := "-Wall -Wextra -Wpedantic -Werror"
cflags := "-Og -g " + std_flags + " " + warn_flags
include_dir := "include"
format_files := "src/arena.c include/arena/arena.h src/tests.c examples/*.c"

default: tests

tests:
    {{cc}} {{cflags}} -I{{include_dir}} -c -o arena.o src/arena.c
    {{cc}} {{cflags}} -Wno-newline-eof -I{{include_dir}} -o test src/tests.c src/rktest.c arena.o -lm

test: tests
    echo "Running tests under valgrind..."
    valgrind ./test
    just clean
    echo "Testing complete."

clean:
    echo "Removing executables..."
    rm -rf .zig-cache
    rm -rf zig-out
    rm -f test
    rm -f *.o
    for src in examples/*.c; do \
        name="$${src##*/}"; \
        name="$${name%.c}"; \
        rm -f "$${name}"; \
    done
    echo "Executables removed."

format:
    zig fmt build.zig
    {{clang_format}} -i {{format_files}}

fmt: format
