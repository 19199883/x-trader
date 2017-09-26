The bowsprit-test program uses some mocked timestamps and hard-coded
measurements to test the file rollover capabilities.  Eventually we might want
to make the various knobs configurable, and let the caller control what
statistics to make and when to increment them.  That way we can have a wider
variety of test cases.  But this works fine for now.

  $ bowsprit-test
  $ for file in $(find . -name 'stats*.log' | sort); do
  >     echo
  >     echo $file
  >     cat $file
  > done
  
  ./stats-20140101-000000-0000.log
  === Statistics of as of 2014-01-01 00:00:00
  0              test-1/dog
  0              test-1/cat
  0              test-1/frog
  === Statistics of as of 2014-01-01 00:00:30
  1              test-1/dog
  2    0.067  /s test-1/cat
  1              test-1/frog
  
  ./stats-20140101-000100-0001.log
  === Statistics of as of 2014-01-01 00:01:00
  2              test-1/dog
  4    0.067  /s test-1/cat
  2              test-1/frog
  === Statistics of as of 2014-01-01 00:01:30
  4              test-1/dog
  5    0.033  /s test-1/cat
  3              test-1/frog
  
  ./stats-slugged-20140101-000000-0000.log
  === Statistics of as of 2014-01-01 00:00:00
  0              test-1/dog
  0              test-1/cat
  0              test-1/frog
  === Statistics of as of 2014-01-01 00:00:30
  1              test-1/dog
  2    0.067  /s test-1/cat
  1              test-1/frog
  
  ./stats-slugged-20140101-000100-0001.log
  === Statistics of as of 2014-01-01 00:01:00
  2              test-1/dog
  4    0.067  /s test-1/cat
  2              test-1/frog
  === Statistics of as of 2014-01-01 00:01:30
  4              test-1/dog
  5    0.033  /s test-1/cat
  3              test-1/frog
