The bowsprit-test program uses some mocked timestamps and hard-coded
measurements to test the periodic interval checking.  Eventually we might want
to make the various knobs configurable, and let the caller control what
statistics to make and when to increment them.  That way we can have a wider
variety of test cases.  But this works fine for now.

  $ CLOG=INFO bowsprit-test
  [INFO    ] bowsprit-test: Statistics as of 2014-01-01 00:00:00
  [INFO    ] bowsprit-test: 0              test-1/dog
  [INFO    ] bowsprit-test: 0              test-1/cat
  [INFO    ] bowsprit-test: 0              test-1/frog
  [INFO    ] bowsprit-test: Statistics as of 2014-01-01 00:00:30
  [INFO    ] bowsprit-test: 1              test-1/dog
  [INFO    ] bowsprit-test: 2    0.067  /s test-1/cat
  [INFO    ] bowsprit-test: 1              test-1/frog
  [INFO    ] bowsprit-test: Statistics as of 2014-01-01 00:01:00
  [INFO    ] bowsprit-test: 2              test-1/dog
  [INFO    ] bowsprit-test: 4    0.067  /s test-1/cat
  [INFO    ] bowsprit-test: 2              test-1/frog
  [INFO    ] bowsprit-test: Statistics as of 2014-01-01 00:01:30
  [INFO    ] bowsprit-test: 4              test-1/dog
  [INFO    ] bowsprit-test: 5    0.033  /s test-1/cat
  [INFO    ] bowsprit-test: 3              test-1/frog
