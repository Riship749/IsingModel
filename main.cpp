#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include "include/IsingLattice.h"
#include "include/MonteCarlos.h"

int main() {
    int L = 32;
    const double PI = std::acos(-1.0);

    //list of the active fractions we want to test (1.0 = 0% defects, 0.7 = 30% defects)
    std::vector<double> activeFractions = {1.0, 0.90, 0.80, 0.70};

    double omega = 2.0 * PI * 0.01;
    int stepsPerPeriod = 200;
    int transientPeriods = 50;  // High burn-in to handle the 30% defect lag
    int dataPeriods = 20;

    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Automated Defect Scaling Batch..." << std::endl;

    //Iterate through each defect concentration
    for (double activeFraction : activeFractions) {

        // Calculate the defect percentage for the filename (e.g., 10, 20, 30)
        int defectPercent = std::round((1.0 - activeFraction) * 100);
        std::string filename = "data/dynamic_boundary_" + std::to_string(defectPercent) + "pct.csv";

        std::ofstream outFile(filename);
        outFile << "Temperature,H0,Q_OrderParameter\n";

        std::cout << "\n=== Running " << defectPercent << "% Defects ===" << std::endl;

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

    std::cout << "\nAll batch simulations complete!" << std::endl;
    return 0;
}