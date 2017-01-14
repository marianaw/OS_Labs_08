/* $Date: 2008-10-31 09:14:09 -0200 (Fri, 31 Oct 2008) $ */
/* $Revision: 682 $ */

#include "log.h"
#include <stdarg.h>

static int loglevel=0;
static FILE *logfile=NULL;

int log_open(char *filename,int ll)
{
	loglevel=0;
	if(filename == NULL) logfile = stdout;
	else logfile = fopen(filename,"w");
	if(logfile == NULL) return 1;
	loglevel = ll;
	return 0;
}

void log_close(void)
{
	fclose(logfile);
}

void debug1(const char * format, ...)
{
	va_list args;
	if(loglevel < 1 || logfile == NULL) return;
	va_start(args, format);
	vfprintf(logfile,format,args);
	va_end(args);
}

void debug2(const char *format, ...)
{
	va_list args;
	if(loglevel < 2 || logfile == NULL) return;
	va_start(args, format);
	vfprintf(logfile,format,args);
	va_end(args);
}

void debug3(const char *format, ...)
{
	va_list args;
	if(loglevel < 3 || logfile == NULL) return;
	va_start(args, format);
	vfprintf(logfile,format,args);
	va_end(args);
}
