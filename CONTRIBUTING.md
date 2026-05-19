# Contributing to ChronoTrade

Thank you for your interest in contributing to ChronoTrade! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Requirements](#testing-requirements)
- [Performance Requirements](#performance-requirements)
- [Submitting Changes](#submitting-changes)
- [Review Process](#review-process)

## Code of Conduct

This project adheres to professional standards of conduct:

- Be respectful and constructive in all interactions
- Focus on technical merit and project goals
- Welcome newcomers and help them get started
- Provide actionable feedback in code reviews
- Respect different perspectives and experience levels

## Getting Started

### Prerequisites

- C++17 or later compiler (GCC 12+, Clang 15+, MSVC 2022+)
- CMake 3.20+
- Git
- CUDA Toolkit 12.0+ (optional, for GPU features)
- Google Test and Google Benchmark (for testing)

### Setting Up Development Environment

1. **Fork and clone the repository:**
   ```bash
   git clone https://github.com/Brijes987/ChronoTrade
   cd ChronoTrade
   ```

2. **Create a development branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_FULL_PROJECT=ON ..
   make -j$(nproc)
   ```

4. **Run tests:**
   ```bash
   ctest --output-on-failure
   ```

## Development Workflow

### Branch Naming Convention

- `feature/description` - New features
- `fix/description` - Bug fixes
- `perf/description` - Performance improvements
- `refactor/description` - Code refactoring
- `docs/description` - Documentation updates
- `test/description` - Test additions or improvements

### Commit Message Format

Follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `perf`: Performance improvement
- `refactor`: Code refactoring
- `test`: Test additions or modifications
- `docs`: Documentation changes
- `chore`: Build process or auxiliary tool changes
- `style`: Code style changes (formatting, missing semicolons, etc.)

**Examples:**
```
feat(matching): add iceberg order support

Implement iceberg order functionality that allows large orders
to be split into smaller visible portions.

Closes #123
```

```
perf(orderbook): optimize price level lookup using custom allocator

Replace std::map with custom B-tree implementation for 15% latency
reduction in order insertion.

Benchmark results:
- Before: 512ns P99
- After: 437ns P99
```

## Coding Standards

### C++ Style Guide

We follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/) with these specific rules:

#### Naming Conventions

```cpp
// Classes and structs: PascalCase
class OrderBook {};
struct PriceLevel {};

// Functions and methods: snake_case
void process_order();
bool is_valid_price();

// Variables: snake_case
int order_count;
Price best_bid;

// Constants: UPPER_SNAKE_CASE
constexpr int MAX_ORDERS = 10000;
const Price MIN_PRICE = 100;

// Template parameters: PascalCase
template<typename T, size_t Capacity>
class LockFreeQueue {};

// Namespaces: snake_case
namespace trading_engine::matching_engine {}
```

#### Code Formatting

- **Indentation**: 4 spaces (no tabs)
- **Line length**: 100 characters maximum
- **Braces**: Opening brace on same line (K&R style)
- **Spacing**: Space after keywords, around operators

```cpp
// Good
if (condition) {
    do_something();
} else {
    do_something_else();
}

// Bad
if(condition)
{
    do_something();
}
else
{
    do_something_else();
}
```

#### Performance-Critical Code

For hot paths (order matching, market data processing):

```cpp
// Use noexcept where possible
bool try_match_order(Order* order) noexcept;

// Prefer const references for parameters
void process_trade(const Trade& trade);

// Use [[likely]] and [[unlikely]] for branch prediction
if (order.is_valid()) [[likely]] {
    process_order(order);
}

// Avoid allocations in hot paths
// Use memory pools or stack allocation
```

#### Memory Management

```cpp
// Prefer RAII and smart pointers
std::unique_ptr<Order> order = std::make_unique<Order>();

// Use custom allocators for performance-critical paths
memory::PoolAllocator<Order> order_allocator;

// Avoid raw new/delete in application code
// Exception: Lock-free data structures with careful ownership
```

#### Concurrency

```cpp
// Use std::atomic for lock-free operations
std::atomic<uint64_t> order_count{0};

// Specify memory ordering explicitly
count.fetch_add(1, std::memory_order_release);

// Document thread safety in comments
// Thread-safe: Can be called from multiple threads
// Not thread-safe: Must be externally synchronized
```

### Code Review Checklist

Before submitting a PR, ensure:

- [ ] Code compiles without warnings (`-Wall -Wextra -Werror`)
- [ ] All tests pass
- [ ] New code has unit tests (target: 80%+ coverage)
- [ ] Performance-critical code has benchmarks
- [ ] No performance regressions (run benchmarks)
- [ ] Code follows style guidelines
- [ ] Documentation is updated
- [ ] Commit messages follow convention
- [ ] No debug code or commented-out code
- [ ] Memory leaks checked (valgrind/sanitizers)

## Testing Requirements

### Unit Tests

All new features must include unit tests:

```cpp
#include <gtest/gtest.h>
#include "trading_engine/matching_engine/order_book.hpp"

TEST(OrderBookTest, AddBuyOrder) {
    OrderBook book("AAPL");
    Order order = create_test_order(Side::Buy, 15000, 100);
    
    OrderId id = book.add_order(order);
    
    EXPECT_NE(id, 0);
    EXPECT_EQ(book.get_order_count(), 1);
}
```

### Benchmark Tests

Performance-critical code must include benchmarks:

```cpp
#include <benchmark/benchmark.h>
#include "trading_engine/matching_engine/order_book.hpp"

static void BM_OrderInsertion(benchmark::State& state) {
    OrderBook book("AAPL");
    
    for (auto _ : state) {
        Order order = create_test_order(Side::Buy, 15000, 100);
        benchmark::DoNotOptimize(book.add_order(order));
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_OrderInsertion);
```

### Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
./tests/test_order_book

# Run with sanitizers
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON ..
make && ctest

# Run benchmarks
./benchmarks/matching_engine_benchmark
```

## Performance Requirements

ChronoTrade is a high-performance system. All contributions must maintain or improve performance:

### Latency Requirements

- Order matching: < 500ns P99
- Market data processing: < 100ns per message
- Risk calculations: < 50μs for 10K positions

### Benchmarking Changes

Before submitting performance-related PRs:

1. **Baseline measurement:**
   ```bash
   git checkout main
   ./scripts/run_benchmarks.sh > baseline.txt
   ```

2. **Measure your changes:**
   ```bash
   git checkout feature/your-feature
   ./scripts/run_benchmarks.sh > feature.txt
   ```

3. **Compare results:**
   ```bash
   python scripts/compare_benchmarks.py baseline.txt feature.txt
   ```

4. **Include results in PR description**

### Performance Regression Policy

- **No regressions** in hot paths (order matching, market data)
- **< 5% regression** acceptable for cold paths with good justification
- **> 5% regression** requires architectural discussion

## Submitting Changes

### Pull Request Process

1. **Update your branch:**
   ```bash
   git checkout main
   git pull upstream main
   git checkout feature/your-feature
   git rebase main
   ```

2. **Run full test suite:**
   ```bash
   ./scripts/run_all_tests.sh
   ```

3. **Push to your fork:**
   ```bash
   git push origin feature/your-feature
   ```

4. **Create pull request:**
   - Use descriptive title
   - Fill out PR template completely
   - Link related issues
   - Include benchmark results for performance changes
   - Add screenshots for UI changes (if applicable)

### Pull Request Template

Your PR description should include:

```markdown
## Description
Brief description of changes

## Motivation
Why is this change needed?

## Changes
- List of specific changes
- Organized by component

## Testing
- Unit tests added/modified
- Manual testing performed
- Benchmark results (if applicable)

## Performance Impact
- Latency: [no change | improved | regressed]
- Throughput: [no change | improved | regressed]
- Memory: [no change | improved | regressed]

## Checklist
- [ ] Tests pass
- [ ] Benchmarks run
- [ ] Documentation updated
- [ ] No performance regression
```

## Review Process

### What to Expect

1. **Initial review**: Within 2-3 business days
2. **Feedback**: Constructive comments on code, design, performance
3. **Iteration**: Address feedback, push updates
4. **Approval**: At least one maintainer approval required
5. **Merge**: Maintainer will merge after approval

### Review Criteria

Reviewers will check:

- **Correctness**: Does the code work as intended?
- **Performance**: Does it meet latency/throughput requirements?
- **Testing**: Are tests comprehensive and meaningful?
- **Style**: Does it follow coding standards?
- **Documentation**: Is it well-documented?
- **Design**: Is the design sound and maintainable?

### Addressing Feedback

- Respond to all comments (even if just "Done")
- Push new commits for each round of feedback
- Don't force-push after review starts
- Mark conversations as resolved when addressed

## Getting Help

- **Questions**: Open a GitHub Discussion
- **Bugs**: Open a GitHub Issue
- **Security**: Email security@chronotrade.example.com (do not open public issue)
- **Chat**: Join our Discord server (link in README)

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Mentioned in release notes
- Credited in commit history

Significant contributions may result in:
- Maintainer status
- Speaking opportunities at conferences
- Co-authorship on papers

---

Thank you for contributing to ChronoTrade! Your efforts help make this project better for everyone.
