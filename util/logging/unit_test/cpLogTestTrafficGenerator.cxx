#include "stdio.h"
#include "cpLog.h"

int main()
{
    char* message = "All work and no play makes Quinny a dull boy.";
    if (! cpLogOpen ("/usr/local/vocal/log/test"))
    {
	printf ("Could not open the file\n");
    }

    while (1)
    {
	cpLog (LOG_ERR, message);
    }
}
