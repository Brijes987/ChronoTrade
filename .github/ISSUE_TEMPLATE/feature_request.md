---
name: Feature Request
about: Suggest a new feature or enhancement
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

## Feature Description

A clear and concise description of the feature you'd like to see.

## Motivation

Why is this feature needed? What problem does it solve?

## Proposed Solution

Describe how you envision this feature working.

## Alternative Solutions

Have you considered any alternative approaches? If so, describe them here.

## Use Case

Provide a concrete example of how this feature would be used:

```cpp
// Example code showing the proposed API or usage
OrderBook book("AAPL");
book.add_iceberg_order(order, visible_quantity=100);
```

## Performance Considerations

- **Expected latency impact**: [e.g., +50ns, negligible, unknown]
- **Memory impact**: [e.g., +10MB per order book, minimal]
- **Throughput impact**: [e.g., no impact, -5% for this operation]

## Implementation Complexity

- [ ] Simple (< 1 day)
- [ ] Medium (1-3 days)
- [ ] Complex (> 3 days)
- [ ] Requires architectural changes

## Breaking Changes

- [ ] This feature requires breaking API changes
- [ ] This feature is backward compatible

## Additional Context

Add any other context, mockups, or examples about the feature request here.

## Related Issues

Link to any related issues or discussions:
- #123
- #456

## Checklist

- [ ] I have searched existing issues to ensure this is not a duplicate
- [ ] I have described the motivation clearly
- [ ] I have considered performance implications
- [ ] I have provided a concrete use case
