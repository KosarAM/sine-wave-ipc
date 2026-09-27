#include "SharedSemaphore.h"

#include <fcntl.h>
#include <iostream>

SharedSemaphore::SharedSemaphore(const char* name)
    : name(name),
      semaphore(nullptr),
      owner(false)
{
}

SharedSemaphore::~SharedSemaphore()
{
    if (semaphore != nullptr)
    {
        sem_close(semaphore);
    }

    if (owner)
    {
        sem_unlink(name);
    }
}

bool SharedSemaphore::create()
{
    semaphore = sem_open(
        name,
        O_CREAT | O_EXCL,
        0666,
        1
    );

    if (semaphore == SEM_FAILED)
    {
        semaphore = nullptr;

        // اگر قبلاً وجود داشته، بازش کن
        semaphore = sem_open(
            name,
            0
        );

        if (semaphore == SEM_FAILED)
        {
            semaphore = nullptr;
            perror("sem_open");
            return false;
        }

        owner = false;
        return true;
    }

    owner = true;
    return true;
}

bool SharedSemaphore::open()
{
    semaphore = sem_open(
        name,
        0
    );

    if (semaphore == SEM_FAILED)
    {
        semaphore = nullptr;
        perror("sem_open");
        return false;
    }

    return true;
}

void SharedSemaphore::lock()
{
    if (semaphore != nullptr)
    {
        sem_wait(semaphore);
    }
}

void SharedSemaphore::unlock()
{
    if (semaphore != nullptr)
    {
        sem_post(semaphore);
    }
}