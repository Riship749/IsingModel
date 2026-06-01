#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <sstream>
#include "include/IsingLattice.h"
#include "include/MonteCarlos.h"

int main() {
    const double pi = std::acos(-1.0);
    int L = 32;

    // LOCK IN SPACE: Fix the lattice at 10% Quenched Defects
    double activeFraction = 0.90;

    //A list of the driving frequencies (omega) we want to test
    // 0.01*pi (Slow), 0.05*pi (Medium), 0.10*pi (Fast)
    std::vector<double> omegas = {0.01 * pi, 0.05 * pi, 0.10 * pi};
    std::vector<std::string> omega_labels = {"0.01", "0.05", "0.10"};

    int stepsPerPeriod = 200;
    int transientPeriods = 50;
    int dataPeriods = 20;

    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Automated Frequency Scaling Batch..." << std::endl;

    // THE BATCH LOOP: Iterate through each driving frequency
    for (size_t i = 0; i < omegas.size(); ++i) {

        double omega = omegas[i];
        std::string filename = "data/dynamic_boundary_w" + omega_labels[i] + ".csv";

        std::ofstream outFile(filename);
        outFile << "Temperature,H0,Q_OrderParameter\n";

        std::cout << "\n=== Running Frequency omega = " << omega_labels[i] << "*pi ===" << std::endl;

        // Create the lattice once per frequency so the 10% defects remain constant
        IsingLattice lattice(L);
        lattice.initializeDefects(rng, activeFraction);

        for (double H0 = hStart; H0 <= hEnd; H0 += hStep) {
            std::cout << "  Simulating Field Amplitude H0 = " << std::fixed << std::setprecision(1) << H0 << std::endl;

            lattice.initializeRandom(rng);
            MonteCarlos mc(lattice, 42);

            for (double T = tStart; T >= tEnd; T -= tStep) {

                DynamicObservables obs = mc.simulateDynamic(T, H0, omega, dataPeriods, stepsPerPeriod, transientPeriods);
                double absQ = std::abs(obs.Q);

                outFile << std::fixed << std::setprecision(4)
                        << T << ","
                        << H0 << ","
                        << absQ << "\n";
            }
        }
        outFile.close();
        std::cout << "Saved: " << filename << std::endl;
    }

    std::cout << "\nAll frequency simulations complete!" << std::endl;
    return 0;
}