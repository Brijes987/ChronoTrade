---
name: Performance Issue
about: Report a performance regression or optimization opportunity
title: '[PERF] '
labels: performance
assignees: ''
---

## Performance Issue Description

A clear description of the performance problem or optimization opportunity.

## Current Performance

Provide benchmark results showing current performance:

```
Benchmark                          Time             CPU   Iterations
------------------------------------------------------------------------
BM_OrderInsertion               512 ns          511 ns      1367925
BM_OrderCancellation            203 ns          203 ns      3447783
```

## Expected Performance

What performance level do you expect or need?

```
Expected: < 400ns P99 for order insertion
Actual: 512ns P99
```

## Regression Information

If this is a regression:
- **Last known good version/commit**: [e.g., v0.9.0, commit abc123]
- **First bad version/commit**: [e.g., v1.0.0, commit def456]
- **Suspected cause**: [e.g., "Added validation in hot path"]

## Reproduction Steps

How to reproduce the performance issue:

1. Build with: `cmake -DCMAKE_BUILD_TYPE=Release ..`
2. Run benchmark: `./benchmarks/matching_engine_benchmark`
3. Observe latency: `P99 = 512ns`

## Environment

- **CPU**: [e.g., AMD Ryzen 7 5800X, Intel Xeon E5-2680]
- **RAM**: [e.g., 32GB DDR4-3200]
- **OS**: [e.g., Ubuntu 22.04]
- **Compiler**: [e.g., GCC 12.2 with -O3 -march=native]
- **CUDA**: [e.g., 12.0, or N/A]

## Profiling Data

If you have profiling data, include it here:

```
perf report output, flamegraph, or other profiling data
```

## Proposed Solution

If you have ideas for optimization:

- [ ] Reduce allocations
- [ ] Improve cache locality
- [ ] Use SIMD instructions
- [ ] Optimize algorithm
- [ ] Reduce lock contention
- [ ] Other: [describe]

**Details**: [Explain your proposed optimization]

## Impact Assessment

- **Affected operations**: [e.g., order insertion, market data processing]
- **Frequency**: [e.g., hot path, called millions of times per second]
- **Business impact**: [e.g., increases latency by 20%, reduces throughput by 10%]

## Benchmark Comparison

If you have a fix, provide before/after benchmarks:

### Before
```
BM_OrderInsertion               512 ns          511 ns      1367925
```

### After
```
BM_OrderInsertion               387 ns          386 ns      1812345
```

**Improvement**: 24% faster

## Additional Context

Add any other context about the performance issue here.

## Checklist

- [ ] I have run benchmarks to quantify the issue
- [ ] I have profiled the code to identify bottlenecks
- [ ] I have checked for similar issues
- [ ] I have provided environment details
- [ ] I have included benchmark results
