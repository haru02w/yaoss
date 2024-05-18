#include "ui/ui.h"
#include "core/kernel.h"
#include <stdlib.h>
extern struct kernel kernel;
int main(void)
{
    run_curses();
    return EXIT_SUCCESS;
}
