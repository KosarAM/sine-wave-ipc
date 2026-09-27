#include "SharedMemory.h"

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

SharedMemory::SharedMemory(const char* name)
    : name(name),
      shmFd(-1),
      sharedData(nullptr)
{
}

SharedMemory::~SharedMemory()
{
    if (sharedData != nullptr)
    {
        munmap(
            sharedData,
            sizeof(SharedData)
        );
    }

    if (shmFd != -1)
    {
        close(shmFd);
    }

    shm_unlink(name);
}

bool SharedMemory::create()
{
    shmFd = shm_open(
        name,
        O_CREAT | O_RDWR,
        0666
    );

    if (shmFd == -1)
    {
        perror("shm_open");
        return false;
    }

    if (ftruncate(
            shmFd,
            sizeof(SharedData)
        ) == -1)
    {
        perror("ftruncate");
        return false;
    }

    sharedData = static_cast<SharedData*>(
        mmap(
            nullptr,
            sizeof(SharedData),
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            shmFd,
            0
        )
    );

    if (sharedData == MAP_FAILED)
    {
        perror("mmap");
        sharedData = nullptr;
        return false;
    }

    return true;
}

SharedData* SharedMemory::data()
{
    return sharedData;
}