#include "cpLog.h"

void print_some_errors()
{
    cpLog (LOG_ERR, "This is an error message.  You should see it.");

    /* boundary condition */
    cpLog (LOG_WARNING, "This is a warning message.  You should see it.");

    cpLog (LOG_DEBUG_STACK, "This is a stack debug message.  You should not see it.");
}

int main()
{
    cpLogSetPriority (LOG_WARNING);

    /* Default:  Use standard error */
    print_some_errors();
    /* Next try printing to a file. */
    cpLogOpen ("test_log");
    print_some_errors();
    
    return 0;
}
