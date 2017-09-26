Test the default format.

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


Try some custom formats.

  $ CLOG=TRACE CLOG_FORMAT="[%L] [%l]" clog-test
  [CRITICAL] [CRITICAL]
  [ERROR   ] [ERROR]
  [WARNING ] [WARNING]
  [NOTICE  ] [NOTICE]
  [INFO    ] [INFO]
  [DEBUG   ] [DEBUG]
  [TRACE   ] [TRACE]
  [CRITICAL] [CRITICAL]
  [ERROR   ] [ERROR]
  [WARNING ] [WARNING]
  [NOTICE  ] [NOTICE]
  [INFO    ] [INFO]
  [DEBUG   ] [DEBUG]
  [TRACE   ] [TRACE]

  $ CLOG=TRACE CLOG_FORMAT="%l, %c, %m" clog-test
  CRITICAL, main, This is a critical message
  ERROR, main, This is a error message
  WARNING, main, This is a warning message
  NOTICE, main, This is a notice message
  INFO, main, This is a info message
  DEBUG, main, This is a debug message
  TRACE, main, This is a trace message
  CRITICAL, libclogger, This is a critical message
  ERROR, libclogger, This is a error message
  WARNING, libclogger, This is a warning message
  NOTICE, libclogger, This is a notice message
  INFO, libclogger, This is a info message
  DEBUG, libclogger, This is a debug message
  TRACE, libclogger, This is a trace message

  $ CLOG=TRACE CLOG_FORMAT="[%L] %m" clog-test
  [CRITICAL] This is a critical message
  [ERROR   ] This is a error message
  [WARNING ] This is a warning message
  [NOTICE  ] This is a notice message
  [INFO    ] This is a info message
  [DEBUG   ] This is a debug message
  [TRACE   ] This is a trace message
  [CRITICAL] This is a critical message
  [ERROR   ] This is a error message
  [WARNING ] This is a warning message
  [NOTICE  ] This is a notice message
  [INFO    ] This is a info message
  [DEBUG   ] This is a debug message
  [TRACE   ] This is a trace message

  $ CLOG=TRACE CLOG_FORMAT="logging! [%L] %m" clog-test
  logging! [CRITICAL] This is a critical message
  logging! [ERROR   ] This is a error message
  logging! [WARNING ] This is a warning message
  logging! [NOTICE  ] This is a notice message
  logging! [INFO    ] This is a info message
  logging! [DEBUG   ] This is a debug message
  logging! [TRACE   ] This is a trace message
  logging! [CRITICAL] This is a critical message
  logging! [ERROR   ] This is a error message
  logging! [WARNING ] This is a warning message
  logging! [NOTICE  ] This is a notice message
  logging! [INFO    ] This is a info message
  logging! [DEBUG   ] This is a debug message
  logging! [TRACE   ] This is a trace message

  $ CLOG=TRACE CLOG_FORMAT="not helpful" clog-test
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful
  not helpful


We should get an error message for illegal formats.

  $ CLOG_FORMAT=%q clog-test
  Unknown conversion %q
  [1]
