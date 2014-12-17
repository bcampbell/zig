#ifndef LOG_H
#define LOG_H

extern bool log_open(const char* outfile);
// write to log
extern void log_infof(const char* fmt, ... );

// write to log and stderr
extern void log_errorf(const char* fmt, ... );
extern void log_close();

#endif // LOG_H
