# Pull Request

## Description

Brief description of what this PR does.

## Type of Change

- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Performance improvement (non-breaking change which improves performance)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update
- [ ] Code refactoring
- [ ] Test additions or improvements

## Motivation and Context

Why is this change required? What problem does it solve?

Fixes #(issue number)

## Changes Made

List the specific changes made in this PR:

- Added X functionality to Y component
- Refactored Z for better performance
- Updated documentation for A

## Testing

### Unit Tests

- [ ] All existing tests pass
- [ ] New tests added for new functionality
- [ ] Test coverage maintained or improved

**Test Results:**
```
[==========] Running 42 tests from 8 test suites.
[==========] 42 tests from 8 test suites ran. (1234 ms total)
[  PASSED  ] 42 tests.
```

### Manual Testing

Describe the manual testing you performed:

1. Built project with Release configuration
2. Ran trading engine with test orders
3. Verified order matching works correctly
4. Checked logs for errors

### Benchmark Results

If this PR affects performance, include benchmark results:

#### Before
```
Benchmark                          Time             CPU   Iterations
------------------------------------------------------------------------
BM_OrderInsertion               512 ns          511 ns      1367925
BM_OrderCancellation            203 ns          203 ns      3447783
```

#### After
```
Benchmark                          Time             CPU   Iterations
------------------------------------------------------------------------
BM_OrderInsertion               387 ns          386 ns      1812345
BM_OrderCancellation            156 ns          156 ns      4487234
```

**Performance Impact:**
- Order insertion: 24% faster ✅
- Order cancellation: 23% faster ✅

## Performance Impact

- [ ] No performance impact
- [ ] Performance improved (include benchmarks above)
- [ ] Minor performance regression (< 5%, justified below)
- [ ] Performance regression (> 5%, requires discussion)

**Justification for regression (if applicable):**

## Memory Impact

- [ ] No memory impact
- [ ] Memory usage reduced
- [ ] Memory usage increased (justified below)

**Justification (if applicable):**

## Breaking Changes

- [ ] This PR introduces breaking changes

**Breaking changes description:**

## Documentation

- [ ] Documentation updated (README, API docs, etc.)
- [ ] Code comments added/updated
- [ ] CHANGELOG.md updated
- [ ] No documentation needed

## Code Quality

- [ ] Code follows project style guidelines
- [ ] Self-review performed
- [ ] Comments added for complex logic
- [ ] No compiler warnings
- [ ] clang-tidy checks pass
- [ ] No memory leaks (checked with valgrind/sanitizers)

## Checklist

- [ ] My code follows the coding standards of this project
- [ ] I have performed a self-review of my own code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have made corresponding changes to the documentation
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] New and existing unit tests pass locally with my changes
- [ ] Any dependent changes have been merged and published
- [ ] I have run benchmarks and verified no performance regression
- [ ] I have updated CHANGELOG.md

## Screenshots (if applicable)

Add screenshots to help explain your changes (for UI changes, graphs, dashboards, etc.)

## Additional Notes

Any additional information that reviewers should know:

- Dependencies added/removed
- Configuration changes required
- Migration steps needed
- Known issues or limitations

## Reviewer Notes

**Areas that need special attention:**

- [ ] Lock-free algorithm correctness
- [ ] Memory ordering in concurrent code
- [ ] Performance-critical hot paths
- [ ] API design and backward compatibility
- [ ] Error handling and edge cases

---

**For Reviewers:**

Please verify:
1. Code correctness and logic
2. Performance impact (check benchmarks)
3. Test coverage and quality
4. Documentation completeness
5. Code style and readability
6. Thread safety and concurrency
7. Memory management and leaks
