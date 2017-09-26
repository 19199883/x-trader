Test the default (which should print out all channels).

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


Try limiting ourselves to one channel

  $ CLOG=TRACE CLOG_CHANNELS=main clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [DEBUG   ] main: This is a debug message
  [TRACE   ] main: This is a trace message

  $ CLOG=TRACE CLOG_CHANNELS=libclogger clog-test
  [CRITICAL] libclogger: This is a critical message
  [ERROR   ] libclogger: This is a error message
  [WARNING ] libclogger: This is a warning message
  [NOTICE  ] libclogger: This is a notice message
  [INFO    ] libclogger: This is a info message
  [DEBUG   ] libclogger: This is a debug message
  [TRACE   ] libclogger: This is a trace message


Try explicitly asking for both channels.

  $ CLOG=TRACE CLOG_CHANNELS=main,libclogger clog-test
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

  $ CLOG=TRACE CLOG_CHANNELS=libclogger,main clog-test
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


Try including channels that don't exit.

  $ CLOG=TRACE CLOG_CHANNELS=unknown clog-test

  $ CLOG=TRACE CLOG_CHANNELS=unknown,main clog-test
  [CRITICAL] main: This is a critical message
  [ERROR   ] main: This is a error message
  [WARNING ] main: This is a warning message
  [NOTICE  ] main: This is a notice message
  [INFO    ] main: This is a info message
  [DEBUG   ] main: This is a debug message
  [TRACE   ] main: This is a trace message

  $ CLOG=TRACE CLOG_CHANNELS=unknown,main,libclogger clog-test
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
