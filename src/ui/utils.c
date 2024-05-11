#include <time.h>
void sleep(double seconds)
{
    clock_t initial = clock();
    while ((((double)(clock() - initial)) / CLOCKS_PER_SEC) < seconds)
        ;
}
