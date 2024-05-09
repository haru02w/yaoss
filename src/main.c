#include "ui/main.h"
#include <pthread.h>
#include <stdlib.h>

int main(void)
{
    pthread_t /* kernel_thread, */ ui_thread;
    // TODO: pthread_create(&kernel_thread, NULL, &kernel, NULL);
    pthread_create(&ui_thread, NULL, &ui_main, NULL);
    pthread_join(ui_thread, NULL);
    return EXIT_SUCCESS;
}
