#include "../include/MonteCarlos.h"

MonteCarlos::MonteCarlos(IsingLattice& lat, int seed)
    : lattice(lat), rng(seed), realDist(0.0, 1.0), intDist(0, lat.getSize() - 1) {}

// Perform one Monte Carlo Sweep (N flip attempts) at a frozen moment in time
void MonteCarlos::performDynamicSweep(double T, double H_ext, double J) {
    int N = lattice.getNumSpins();

    for (int i = 0; i < N; ++i) {
        int x = intDist(rng);
        int y = intDist(rng);

        if (lattice.getDefect(x, y) == 0) {
            continue;
        }

        double dE = lattice.calculateDeltaE(x, y, H_ext, J);

        // Energy minimization principle
        if (dE <= 0.0) {
            lattice.flipSpin(x, y);
        }
        // Entropy maximization (requires dynamic exponential calculation now)
        else {
            if (realDist(rng) < std::exp(-dE / T)) {
                lattice.flipSpin(x, y);
            }
        }
    }
}

// The Main Physics Loop
DynamicObservables MonteCarlos::simulateDynamic(double T, double H0, double omega,
                                               int periods, int stepsPerPeriod,
                                               int transientPeriods, double J) {

    // Thermalization Phase
    // Let the lattice run for a few periods to lock into a stable limit cycle  -> hysteresis loop
    for (int p = 0; p < transientPeriods; ++p) {
        for (int step = 0; step < stepsPerPeriod; ++step) {
            // Map the discrete step to continuous physical time
            double t = (double)step / stepsPerPeriod * (2.0 * pi / omega);
            double H_ext = H0 * std::sin(omega * t);

            performDynamicSweep(T, H_ext, J);
        }
    }

    // 2. Data Collection Phase
    double sumQ = 0.0; // Accumulator for the dynamic order parameter

    for (int p = 0; p < periods; ++p) {
        double periodMagnetizationSum = 0.0;

        for (int step = 0; step < stepsPerPeriod; ++step) {
            double t = (double)step / stepsPerPeriod * (2.0 * pi / omega);
            double H_ext = H0 * std::sin(omega * t);

            performDynamicSweep(T, H_ext, J);

            // Calculate instantaneous macroscopic magnetization M(t)
            // Note: We do NOT use absolute value here, because we want the signed integral
            double M = (double)lattice.calculateTotalMagnetization() / lattice.getNumSpins();
            periodMagnetizationSum += M;
        }

        // Q for this specific period is the average M(t)
        double Q_period = periodMagnetizationSum / stepsPerPeriod;
        sumQ += std::abs(Q_period);
        }

    // Average Q across all recorded periods to smooth out statistical noise
    DynamicObservables obs;
    obs.Q = sumQ / periods;

    return obs;
}
void MonteCarlos::metropolisSweep(double T, double H0, double omega, double t) {
    int L = lattice.L;
    int N = lattice.N;

    // --- THE FIX: Oscillating Magnetic Field ---
    double H_inst = H0 * std::sin(omega * t);

    std::uniform_int_distribution<int> site_dist(0, N - 1);
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

    for (int step = 0; step < N; ++step) {
        int idx = site_dist(rng);

        if (lattice.defects[idx] == 0) continue;

        int x = idx % L;
        int y = idx / L;

        int right = y * L + (x + 1) % L;
        int left  = y * L + (x - 1 + L) % L;
        int up    = ((y + 1) % L) * L + x;
        int down  = ((y - 1 + L) % L) * L + x;

        int neighbor_sum = lattice.spins[up] + lattice.spins[down] +
                           lattice.spins[left] + lattice.spins[right];

        double current_spin = lattice.spins[idx];
        double delta_E = 2.0 * current_spin * (neighbor_sum + H_inst);

        if (delta_E <= 0.0) {
            lattice.spins[idx] *= -1;
        } else {
            double boltzmann_prob = std::exp(-delta_E / T);
            if (prob_dist(rng) < boltzmann_prob) {
                lattice.spins[idx] *= -1;
            }
        }
    }
}