#ifndef SHAREDDATA_H
#define SHAREDDATA_H

constexpr int MAX_SAMPLES = 1024;

struct SharedData
{
    double samples[MAX_SAMPLES];

    int sampleCount;

    double frequency;
    double amplitude;

    double sampleRate;

    bool running;
};

#endif