#ifndef APPSERVER
#define APPSERVER

#include <pthread.h>

void *worker_loop(void *args);
void bank_loop();

#endif
