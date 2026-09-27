#ifndef SHAREDSEMAPHORE_H
#define SHAREDSEMAPHORE_H

#include <semaphore.h>

class SharedSemaphore
{
public:
    explicit SharedSemaphore(const char* name);
    ~SharedSemaphore();

    bool create();
    bool open();

    void lock();
    void unlock();

private:
    const char* name;
    sem_t* semaphore;
    bool owner;
};

#endif