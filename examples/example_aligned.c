#include <stdio.h> // printf

#define ARENA_IMPLEMENTATION
#include <arena/arena.h>

int main() {
  constexpr size_t arena_size = 1'024; // Allocate a 1kB arena
  auto arena = arena_create(arena_size);
  if (arena == nullptr) {
    return 1;
  }

  [[maybe_unused]] char *char_ptr_1 = arena_alloc_aligned(arena, 10, 4);
  printf("%zu\n", arena->index); // 10

  [[maybe_unused]] char *char_ptr_2 = arena_alloc_aligned(arena, 10, 4);
  printf("%zu\n", arena->index); // 22

  [[maybe_unused]] char *char_ptr_3 = arena_alloc_aligned(arena, 10, 4);
  printf("%zu\n", arena->index); // 34

  arena_destroy(arena); // Free the allocated arena and everything in it

  return 0;
}
