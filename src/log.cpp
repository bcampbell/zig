#include <stdarg.h>
#include <stdio.h>

#include "log.h"


static FILE* logfile = 0;
static bool ownfile = false;

void log_infof(const char* fmt, ... )
{
    if (!logfile)
        return;

    va_list ap;
    va_start(ap, fmt);
    vfprintf( logfile, fmt, ap );
    va_end(ap);
}


void log_errorf(const char* fmt, ... )
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf,sizeof(buf), fmt, ap);
    if (n>=sizeof(buf))
        buf[sizeof(buf)-1]='\0';
    va_end(ap);

    fwrite(buf,1,n,stderr);
    if (logfile)
        fwrite(buf,1,n,logfile);
}






void log_close()
{
    if (!logfile)
       return;
    if (ownfile)
    {
        fclose(logfile);
        ownfile = false;
    }
    logfile = 0;
}


bool log_open( const char* outfile )
{
    log_close();
    if (outfile[0]=='-' && outfile[1]=='\0')
    {
        logfile = stdout;
        ownfile = false;
    }
    else
    {
        logfile = fopen(outfile,"wt");
        if (!logfile) {
            return false;
        }
        ownfile = true;
    }
    //setvbuf(stdout, NULL, _IONBF, 0)  // no buffering
    //setvbuf(stdout, NULL, _IOLBF, 0)  // line buffered (windows: same as fully buffered)
    return true;
}


