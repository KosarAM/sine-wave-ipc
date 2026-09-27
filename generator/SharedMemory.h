#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include "SharedData.h"

class SharedMemory
{
public:
    SharedMemory(const char* name);
    ~SharedMemory();

    bool create();

    SharedData* data();

private:
    const char* name;

    int shmFd;
    SharedData* sharedData;
};

#endif