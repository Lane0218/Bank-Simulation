#ifndef RANDOM_NUMBER_GENERATOR
#define RANDOM_NUMBER_GENERATOR

#include <time.h>

// used to generate random number from current seed and update seed
const unsigned long maxshort = 65536L;
const unsigned long multiplier = 1194211693L;
const unsigned long adder = 12345L;

class RandomNumber
{
private:
    // private member containing current seed
    unsigned long randSeed;

public:
    // constructor. default 0 gives automatic seeding
    RandomNumber(unsigned long s = 0);

    // generate random integer 0 <= value <= n-1
    unsigned short Random(unsigned long n);

    // generate random real number 0 <= value < 1.0
    double fRandom(void);
};

// seed the generator
RandomNumber::RandomNumber(unsigned long s)
{
    if (s == 0)
        randSeed = time(0); // automatic seeding using system time
    else
        randSeed = s; // user supplies the seed
}

// return random integer 0 <= value <= n-1 < 65536
unsigned short RandomNumber::Random(unsigned long n)
{
    randSeed = multiplier * randSeed + adder;
    return (unsigned short)((randSeed >> 16) % n);
}

// return (value in range 0..65535) / 65536
double RandomNumber::fRandom(void)
{
    return Random(maxshort) / double(maxshort);
}

#endif // RANDOM_NUMBER_GENERATOR
