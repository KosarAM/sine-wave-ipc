#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H


#include <vector>


class SignalGenerator
{

public:

    SignalGenerator();


    void setFrequency(double frequency);

    void setAmplitude(double amplitude);


    std::vector<double> generateSamples(
        int numberOfSamples,
        double sampleRate
    );


private:

    double amplitude;

    double frequency;

    double phase;

};


#endif