#pragma once
#include "IsingLattice.h"
#include <random>

// Struct to hold our dynamic results
struct DynamicObservables {
    double Q; // The Dynamic Order Parameter (Time-averaged magnetization over a period)
};

class MonteCarlos {
private:
    IsingLattice& lattice;
    std::mt19937 rng;
    std::uniform_real_distribution<double> realDist;
    std::uniform_int_distribution<int> intDist;

    // The single sweep now requires the instantaneous external field
    void performDynamicSweep(double T, double H_ext, double J = 1.0);

public:
    double pi = std::atan(1.0) * 4.0;

    MonteCarlos(IsingLattice& lat, int seed = 42);

    // Dynamic execution function based on field periods rather than raw MCS
    DynamicObservables simulateDynamic(double T, double H0, double omega, 
                                       int periods, int stepsPerPeriod, 
                                       int transientPeriods, double J = 1.0);
};