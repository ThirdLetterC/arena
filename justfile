set shell := ["bash", "-cu"]

cc := env_var_or_default("CC", "cc")
std_flags := "-std=c2x"
warn_flags := "-Wall -Wextra -Wpedantic -Werror"
cflags := "-Og -g " + std_flags + " " + warn_flags
vendor_dir := "vendor/rktest"
format_files := "arena.c arena.h tests.c code_examples/*.c"

default: tests

tests:
    {{cc}} {{cflags}} -c -o arena.o arena.c
    {{cc}} {{cflags}} -Wno-newline-eof -I{{vendor_dir}} -o test tests.c {{vendor_dir}}/rktest.c arena.o -lm

test: tests
    echo "Running tests under valgrind..."
    valgrind ./test
    just clean
    echo "Testing complete."

example name:
    {{cc}} {{cflags}} -o {{name}} code_examples/{{name}}.c

clean:
    echo "Removing executables..."
    rm -f test
    rm -f *.o
    for src in code_examples/*.c; do \
        name="$${src##*/}"; \
        name="$${name%.c}"; \
        rm -f "$${name}"; \
    done
    echo "Executables removed."

format:
    clang-format -i {{format_files}}

fmt: format

examples:
    for src in code_examples/*.c; do \
        name="$${src##*/}"; \
        name="$${name%.c}"; \
        {{cc}} {{cflags}} -o "$${name}" "$${src}"; \
    done
