#include "ui/ui.h"
#include "core/kernel.h"
#include "core/scheduler.c"
#include <stdio.h>
#include <stdlib.h>
extern struct kernel kernel;
int main(void)
{
    kernel_init();
    syscall(PROCESS_CREATE, "programs/p1");
    kernel_run();
    return EXIT_SUCCESS;
}
