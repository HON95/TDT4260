# TODO

- Should overflowed (zero-valued) deltas be handled specially somewhere?
- Delta correlation search traverses in reverse order, meaning newest to oldest or oldest to newest? The example may be weird.
- Prefetch filtering discard if candidate is same as last prefetch, which always triggers if the last if-statement was triggered.
- Are the right things block aligned?
- Optimize parameters (size VS performance, ish).
