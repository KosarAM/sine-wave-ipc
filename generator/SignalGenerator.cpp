#include "SignalGenerator.h"

#include <cmath>

SignalGenerator::SignalGenerator()
{
    amplitude = 1.0;
    frequency = 10.0;
    phase = 0.0;
}

void SignalGenerator::setFrequency(double f)
{
    frequency = f;
}

void SignalGenerator::setAmplitude(double a)
{
    amplitude = a;
}

std::vector<double> SignalGenerator::generateSamples(
    int numberOfSamples,
    double sampleRate)
{
    std::vector<double> samples;
    samples.reserve(numberOfSamples);

    constexpr double PI = 3.14159265358979323846;

    for (int i = 0; i < numberOfSamples; i++)
    {
        double value =
            amplitude * std::sin(2.0 * PI * frequency *
                                 (static_cast<double>(i) / sampleRate) +
                                 phase);

        samples.push_back(value);

        // حفظ پیوستگی فاز موج
        phase += 2.0 * PI * frequency / sampleRate;

        if (phase >= 2.0 * PI)
        {
            phase -= 2.0 * PI;
        }
    }

    return samples;
}