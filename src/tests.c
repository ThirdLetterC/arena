#include <rktest/rktest.h>

#define ARENA_DEBUG
#include <arena/arena.h>

TEST(arena_init_tests, init) {
  Arena arena;
  constexpr size_t data_size = 256;
  char data[256];
  arena_init(&arena, data, data_size);
  EXPECT_TRUE(arena.region == data);
  EXPECT_LONG_EQ((long)arena.size, (long)data_size);
  EXPECT_LONG_EQ((long)arena.index, 0);
}

TEST(arena_create_tests, basic) {
  constexpr size_t arena_size = 8;
  auto arena = arena_create(arena_size);
  ASSERT_TRUE(arena != nullptr);
  EXPECT_LONG_EQ((long)arena->size, (long)arena_size);
  arena_destroy(arena);
}

TEST(arena_create_tests, zero_bytes) {
  ASSERT_TRUE(arena_create(0) == nullptr);
}

TEST(arena_alloc_tests, basic) {
  constexpr size_t region_size = 256;
  char region[256];
  Arena arena;
  arena_init(&arena, region, region_size);
  auto bytes = arena_alloc(&arena, 8);
  ASSERT_TRUE(bytes != nullptr);
  EXPECT_TRUE(arena.region == bytes);
  EXPECT_LONG_EQ((long)arena.index, 8);
  EXPECT_LONG_EQ((long)arena.allocations, 1);
}

TEST(arena_alloc_aligned_tests, edge_case_tight_space) {
  constexpr size_t region_size = 30;
  char region[30];
  Arena arena;
  arena_init(&arena, region, region_size);
  arena.index = 10;

  void *ptr = arena_alloc_aligned(&arena, 14, 16);

  ASSERT_TRUE(ptr != nullptr);
  EXPECT_LONG_EQ((long)arena.index, 30);
  EXPECT_LONG_EQ((long)arena.allocations, 1);
}
