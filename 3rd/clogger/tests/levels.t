Test the default output (i.e., if you don't specify a log level).

  $ clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message


Test each of the log levels.

  $ CLOG=CRITICAL clog-test
  [CRITICAL] main: This is a critical message
  [CRITICAL] libclogger: This is a critical message

  $ CLOG=ERROR clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message

  $ CLOG=WARNING clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message

  $ CLOG=NOTICE clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message

  $ CLOG=INFO clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message

  $ CLOG=DEBUG clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [DEBUG   ] main: This is a debug message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message
  [DEBUG   ] libclogger: This is a debug message

  $ CLOG=TRACE clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [DEBUG   ] main: This is a debug message
  [TRACE   ] main: This is a trace message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message
  [DEBUG   ] libclogger: This is a debug message
  [TRACE   ] libclogger: This is a trace message


The CLOG environment variable should be case insensitive.

  $ CLOG=critical clog-test
  [CRITICAL] main: This is a critical message
  [CRITICAL] libclogger: This is a critical message

  $ CLOG=error clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message

  $ CLOG=warning clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message

  $ CLOG=notice clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message

  $ CLOG=info clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message

  $ CLOG=debug clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [DEBUG   ] main: This is a debug message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message
  [DEBUG   ] libclogger: This is a debug message

  $ CLOG=trace clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [DEBUG   ] main: This is a debug message
  [TRACE   ] main: This is a trace message
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message
  [DEBUG   ] libclogger: This is a debug message
  [TRACE   ] libclogger: This is a trace message


We should get an error message if the log level isn't recognized.

  $ CLOG=UNKNOWN clog-test
  Unknown CLOG level UNKNOWN
  [1]
