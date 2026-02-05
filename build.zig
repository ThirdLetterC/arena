const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const common_debug_flags: []const []const u8 =
        &[_][]const u8{ "-std=c2x", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Og", "-g" };
    const common_release_flags: []const []const u8 =
        &[_][]const u8{ "-std=c2x", "-Wall", "-Wextra", "-Wpedantic", "-Werror" };
    const common_cflags: []const []const u8 =
        if (optimize == .Debug) common_debug_flags else common_release_flags;

    const test_debug_flags: []const []const u8 =
        &[_][]const u8{ "-std=c2x", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Wno-newline-eof", "-Og", "-g" };
    const test_release_flags: []const []const u8 =
        &[_][]const u8{ "-std=c2x", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Wno-newline-eof" };
    const test_cflags: []const []const u8 =
        if (optimize == .Debug) test_debug_flags else test_release_flags;

    const tests_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    const tests_exe = b.addExecutable(.{
        .name = "test",
        .root_module = tests_module,
    });
    tests_exe.addCSourceFile(.{ .file = b.path("tests.c"), .flags = test_cflags });
    tests_exe.addCSourceFile(.{ .file = b.path("vendor/rktest/rktest.c"), .flags = test_cflags });
    tests_exe.addCSourceFile(.{ .file = b.path("arena.c"), .flags = test_cflags });
    tests_exe.addIncludePath(b.path("vendor/rktest"));
    tests_exe.linkLibC();
    tests_exe.linkSystemLibrary("m");
    b.installArtifact(tests_exe);

    const tests_step = b.step("tests", "Build the test executable");
    tests_step.dependOn(&tests_exe.step);

    const run_tests = b.addRunArtifact(tests_exe);
    if (b.args) |args| {
        run_tests.addArgs(args);
    }
    const test_step = b.step("test", "Run the test executable");
    test_step.dependOn(&run_tests.step);

    const examples_step = b.step("examples", "Build all examples");
    var examples_dir = b.build_root.handle.openDir("code_examples", .{ .iterate = true }) catch |err| {
        std.debug.panic("failed to open code_examples: {s}", .{@errorName(err)});
    };
    defer examples_dir.close();

    var iter = examples_dir.iterate();
    while (true) {
        const entry = iter.next() catch |err| {
            std.debug.panic("failed to iterate code_examples: {s}", .{@errorName(err)});
        } orelse break;

        if (entry.kind != .file) {
            continue;
        }
        if (!std.mem.endsWith(u8, entry.name, ".c")) {
            continue;
        }

        const stem = entry.name[0 .. entry.name.len - 2];
        const example_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        });
        const exe = b.addExecutable(.{
            .name = stem,
            .root_module = example_module,
        });
        const source_path = b.fmt("code_examples/{s}", .{entry.name});
        exe.addCSourceFile(.{ .file = b.path(source_path), .flags = common_cflags });
        exe.linkLibC();
        examples_step.dependOn(&exe.step);
    }
}
