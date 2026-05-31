#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <cmath>
#include "include/IsingLattice.h"
#include "include/MonteCarlos.h"


int main() {
    const double pi = std::atan(1.0) * 4.0;

    // 1. Lattice & Defect Parameters
    int L = 32; // 32x32 is large enough to see the phase transition but fast enough to map the whole grid
    double activeFraction = 0.90; // 10% Quenched Defects (Impurities)

    // 2. Dynamic Field Parameters (H(t) = H0 * sin(omega * t))
    double omega = 2.0 * pi * 0.01; // Low frequency external field
    int stepsPerPeriod = 200;         // Discretization of continuous time
    int transientPeriods = 50;        // Periods to discard while system finds its limit cycle
    int dataPeriods = 20;             // Periods to integrate over to find Q

    // 3. Grid Search Parameters (Mapping the Phase Boundary)
    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Dynamic Phase Transition Mapping with Quenched Defects..." << std::endl;

    // Ensure the data directory exists
    std::ofstream outFile("data/dynamic_boundary.csv");
    outFile << "Temperature,H0,Q_OrderParameter\n";

    // Instantiate the lattice and lock in the quenched defects
    IsingLattice lattice(L);
    lattice.initializeDefects(rng, activeFraction);

    // Outer Loop: Scan across different Magnetic Field Amplitudes
    for (double H0 = hStart; H0 <= hEnd; H0 += hStep) {
        std::cout << "Simulating Field Amplitude H0 = " << std::fixed << std::setprecision(1) << H0 << "..." << std::endl;

        // Re-randomize the spins for a fresh start at high temperature, but KEEP the same defects
        lattice.initializeRandom(rng);
        MonteCarlos mc(lattice, 42);

        // Inner Loop: The Cooling Process
        for (double T = tStart; T >= tEnd; T -= tStep) {

            // Run the dynamic engine
            DynamicObservables obs = mc.simulateDynamic(T, H0, omega, dataPeriods, stepsPerPeriod, transientPeriods);

            // We take the absolute value of Q.
            // If |Q| > 0, it is dynamically ordered (spins resist the field).
            // If |Q| = 0, it is dynamically disordered (spins blindly follow the field).
            double absQ = std::abs(obs.Q);

            // Log the coordinates for the heatmap
            outFile << std::fixed << std::setprecision(4)
                    << T << ","
                    << H0 << ","
                    << absQ << "\n";
        }
    }

    outFile.close();
    std::cout << "Mapping complete. Data saved to data/dynamic_boundary.csv" << std::endl;
    return 0;
}