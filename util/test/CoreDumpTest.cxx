#include "ThreadIf.hxx"
#include <signal.h>
       #include <sys/types.h>
       #include <unistd.h>


class T : public ThreadIf
{
public:
    T() {}
    void thread()
    {
        static int x = 0;
	if(x++ == 1)
             abort();
        while (1)
        {
	}
    }
};

void errorfn(int signum)
{
    int x = fork();
    if(x == 0)
      abort();
    else
      exit(0);
//    cout << signum << endl;
//    signal(SIGABRT, SIG_DFL);
//    abort();
}


int main()
{
//    signal(SIGABRT, errorfn);
    T t1;
    T t2;

    t1.run();
    t2.run();
    t1.join();
    t2.join();
    return 0;
}
