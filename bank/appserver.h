#ifndef APPSERVER
#define APPSERVER

#include <pthread.h>
#include <sys/time.h>

void *worker_loop(void *args);
void bank_loop();

#endif
