#include "rktest/rktest.h"

#define ARENA_DEBUG
#include "arena/arena.h"

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
  arena_clear(&arena);
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
  arena_clear(&arena);
}

TEST(arena_alloc_aligned_tests, rejects_non_power_of_two_alignment) {
  char region[64];
  Arena arena;
  arena_init(&arena, region, sizeof(region));

  void *ptr = arena_alloc_aligned(&arena, 8, 6);

  ASSERT_TRUE(ptr == nullptr);
  EXPECT_LONG_EQ((long)arena.index, 0);
  EXPECT_LONG_EQ((long)arena.allocations, 0);
}

TEST(arena_alloc_aligned_tests, rejects_overflowing_index_updates) {
  char region[1];
  Arena arena;
  arena_init(&arena, region, SIZE_MAX);
  arena.index = SIZE_MAX - 4;

  void *ptr = arena_alloc_aligned(&arena, 8, 8);

  ASSERT_TRUE(ptr == nullptr);
  EXPECT_TRUE(arena.index == SIZE_MAX - 4);
  EXPECT_LONG_EQ((long)arena.allocations, 0);
}

TEST(arena_copy_tests, clamps_source_index_to_source_size) {
  char src_region[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  char dest_region[64];
  Arena src;
  Arena dest;

  arena_init(&src, src_region, sizeof(src_region));
  arena_init(&dest, dest_region, sizeof(dest_region));
  src.index = 16;

  const size_t bytes_copied = arena_copy(&dest, &src);
  EXPECT_LONG_EQ((long)bytes_copied, (long)sizeof(src_region));
  EXPECT_LONG_EQ((long)dest.index, (long)sizeof(src_region));
}
