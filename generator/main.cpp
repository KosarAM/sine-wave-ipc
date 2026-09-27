#include <iostream>
#include <thread>
#include <chrono>

#include "SignalGenerator.h"
#include "SharedMemory.h"
#include "SharedSemaphore.h"

int main()
{
    
    // Create signal generator
    

    SignalGenerator generator;

    
    // Create shared memory
    

    SharedMemory sharedMemory(
        "/sine_shared_memory"
    );

    if (!sharedMemory.create())
    {
        std::cerr
            << "Failed to create shared memory"
            << std::endl;

        return 1;
    }

    
    // Create semaphore
    

    SharedSemaphore semaphore(
        "/sine_semaphore"
    );

    if (!semaphore.create())
    {
        std::cerr
            << "Failed to create semaphore"
            << std::endl;

        return 1;
    }

    
    // Get shared data
    

    SharedData* data =
        sharedMemory.data();

    
    // Signal parameters
    

    double sampleRate = 20000.0;
    
    // Initialize shared memory
    

    semaphore.lock();

    data->sampleCount = 0;
    data->frequency = 10.0;
    data->amplitude = 1.0;
    data->sampleRate = sampleRate;
    data->running = true;

    semaphore.unlock();

    std::cout
        << "Shared memory created."
        << std::endl;

    
    // Main generation loop
    

    while (true)
    {
        
        // Read parameters
        

        semaphore.lock();

        double frequency =
            data->frequency;

        double amplitude =
            data->amplitude;

        bool running =
            data->running;

        semaphore.unlock();

        
        // Check running state
        

        if (!running)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(50)
            );

            continue;
        }

        
        // Update generator
        

        generator.setFrequency(
            frequency
        );

        generator.setAmplitude(
            amplitude
        );

        
        // Generate samples
        

        auto samples =
            generator.generateSamples(
                1024,
                sampleRate
            );

        
        // Write samples
        

        semaphore.lock();

        for (int i = 0;
             i < static_cast<int>(samples.size());
             ++i)
        {
            data->samples[i] =
                samples[i];
        }

        data->sampleCount =
            samples.size();

        semaphore.unlock();

        
        // Debug output
        

        std::cout
            << "Generated: "
            << data->sampleCount
            << " samples, "
            << "frequency = "
            << frequency
            << ", amplitude = "
            << amplitude
            << std::endl;

        
        // Small delay
        

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    return 0;
}