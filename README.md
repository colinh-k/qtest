# TODO
- refactor into test suites/modules
  - each suite/module has a list of test cases which it runs
  - if any test case in the suite fails, the whole suite fails
  - e.g., `QTestCase(Add, Positive) -> Suite(Add) -> {Positive}`
  - this way, we can avoid registering multiple tests within a suite, so we can just say `QTestRegister(Add)`, instead of `QTestRegister(Add, Positive)` and `QTestRegister(Add, Negative)`