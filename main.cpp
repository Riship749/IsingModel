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
#include <numeric>
const double pi = std::acos(-1.0);

//Figure 5. sweep with freq.
int main() {
    const double pi = std::acos(-1.0);
    int L = 32;

    // LOCK IN SPACE: Fix the lattice at 10% Quenched Defects
    double activeFraction = 0.90;

    //A list of the driving frequencies (omega) we want to test
    // 0.01*pi (Slow), 0.05*pi (Medium), 0.10*pi (Fast)
    std::vector<double> omegas = {0.01 * pi, 0.05 * pi, 0.10 * pi, .2 * pi};
    std::vector<std::string> omega_labels = {"0.01", "0.05", "0.10", ".20"};

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


//main to get error bars
/*
int main() {
    int L = 32;

    // Lock parameters to the critical point for baseline error analysis
    double T = 2.27;
    double H0 = 0.0;

    // For rigorous statistical error, we need a massive sample size
    int totalSweeps = 100000;
    int thermalizationSweeps = 10000;

    std::mt19937 rng(42);

    std::string filename = "data/raw_timeseries.csv";
    std::ofstream outFile(filename);

    // CSV Header
    outFile << "Sweep,Magnetization\n";

    std::cout << "Starting Raw Timeseries Extraction..." << std::endl;
    std::cout << "Parameters: L=" << L << ", T=" << T << ", H0=" << H0 << std::endl;

    // Initialize a pure lattice (0% defects) for baseline validation
    IsingLattice lattice(L);
    lattice.initializeDefects(rng, 1.0); // 1.0 active fraction = no impurities
    lattice.initializeRandom(rng);

    MonteCarlos mc(lattice, 42);

    // 1. Thermalization Phase (Discard data to reach equilibrium)
    std::cout << "Thermalizing for " << thermalizationSweeps << " sweeps..." << std::endl;
    for (int i = 0; i < thermalizationSweeps; ++i) {
        // Time t=0.0 since H0=0 (static field)
        mc.metropolisSweep(T, H0, 0.0);
    }

    // 2. Data Collection Phase
    std::cout << "Collecting raw data for " << totalSweeps << " sweeps..." << std::endl;
    for (int step = 1; step <= totalSweeps; ++step) {
        mc.metropolisSweep(T, H0, 0.0);

        // Extract the absolute macroscopic magnetization of the current grid state
        double current_M = std::abs(lattice.calculateTotalMagnetization());

        // Write directly to the CSV
        outFile << step << "," << std::fixed << std::setprecision(6) << current_M << "\n";

        if (step % 10000 == 0) {
            std::cout << "  Recorded " << step << " sweeps..." << std::endl;
        }
    }

    outFile.close();
    std::cout << "\nRaw timeseries successfully saved to: " << filename << std::endl;
    std::cout << "You can now run the Python Block Averaging script on this file." << std::endl;

    return 0;
}
*/
//Code to get heat map:

/*
int main() {
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
    std::ofstream outFile("../data/dynamic_boundary.csv");
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
*/

//Code for sweep from 0% defects to 30%
/*
int main() {
    int L = 32;

    std::vector<double> activeFractions = {1.0, 0.90, 0.80, 0.70};

    double omega = 2.0 * M_PI * 0.01;
    int stepsPerPeriod = 200;
    int transientPeriods = 50;  // High burn-in to handle the 30% defect lag
    int dataPeriods = 20;

    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Automated Defect Scaling Batch..." << std::endl;

    // THE BATCH LOOP: Iterate through each defect concentration
    for (double activeFraction : activeFractions) {

        // Calculate the defect percentage for the filename (e.g., 10, 20, 30)
        int defectPercent = std::round((1.0 - activeFraction) * 100);
        std::string filename = "../data/dynamic_boundary_" + std::to_string(defectPercent) + "pct.csv";

        std::ofstream outFile(filename);
        outFile << "Temperature,H0,Q_OrderParameter\n";

        std::cout << "\n=== Running " << defectPercent << "% Defects ===" << std::endl;

        IsingLattice lattice(L);
        lattice.initializeDefects(rng, activeFraction);

        for (double H0 = hStart; H0 <= hEnd; H0 += hStep) {
            std::cout << "  Simulating Field Amplitude H0 = " << std::fixed << std::setprecision(1) << H0 << std::endl;

            lattice.initializeRandom(rng);
            MonteCarlo mc(lattice, 42);

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
*/

//sweep frequencies

/*
int main() {
    int L = 32;

    // LOCK IN SPACE: Fix the lattice at 10% Quenched Defects
    double activeFraction = 0.90;

    // SWEEP TIME: A list of the driving frequencies (omega) we want to test
    // 0.01*pi (Slow), 0.05*pi (Medium), 0.10*pi (Fast)
    std::vector<double> omegas = {0.01 * M_PI, 0.05 * M_PI, 0.10 * M_PI};
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
        std::string filename = "../data/dynamic_boundary_w" + omega_labels[i] + ".csv";

        std::ofstream outFile(filename);
        outFile << "Temperature,H0,Q_OrderParameter\n";

        std::cout << "\n=== Running Frequency omega = " << omega_labels[i] << "*pi ===" << std::endl;

        // Create the lattice once per frequency so the 10% defects remain constant
        IsingLattice lattice(L);
        lattice.initializeDefects(rng, activeFraction);

        for (double H0 = hStart; H0 <= hEnd; H0 += hStep) {
            std::cout << "  Simulating Field Amplitude H0 = " << std::fixed << std::setprecision(1) << H0 << std::endl;

            lattice.initializeRandom(rng);
            MonteCarlo mc(lattice, 42);

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
*/

//get error bars
/*


int main() {
    int L = 32;

    // Lock parameters to the critical point for baseline error analysis
    double T = 2.27;
    double H0 = 0.0;

    // For rigorous statistical error, we need a massive sample size
    int totalSweeps = 100000;
    int thermalizationSweeps = 10000;

    std::mt19937 rng(42);

    std::string filename = "../data/raw_timeseries.csv";
    std::ofstream outFile(filename);

    // CSV Header
    outFile << "Sweep,Magnetization\n";

    std::cout << "Starting Raw Timeseries Extraction..." << std::endl;
    std::cout << "Parameters: L=" << L << ", T=" << T << ", H0=" << H0 << std::endl;

    // Initialize a pure lattice (0% defects) for baseline validation
    IsingLattice lattice(L);
    lattice.initializeDefects(rng, 1.0); // 1.0 active fraction = no impurities
    lattice.initializeRandom(rng);

    MonteCarlo mc(lattice, 42);

    // 1. Thermalization Phase (Discard data to reach equilibrium)
    std::cout << "Thermalizing for " << thermalizationSweeps << " sweeps..." << std::endl;
    for (int i = 0; i < thermalizationSweeps; ++i) {
        // Time t=0.0 since H0=0 (static field)
        mc.metropolisSweep(T, H0, 0.0);
    }

    // 2. Data Collection Phase
    std::cout << "Collecting raw data for " << totalSweeps << " sweeps..." << std::endl;
    for (int step = 1; step <= totalSweeps; ++step) {
        mc.metropolisSweep(T, H0, 0.0);

        // Extract the absolute macroscopic magnetization of the current grid state
        double current_M = std::abs(lattice.calculateMagnetization());

        // Write directly to the CSV
        outFile << step << "," << std::fixed << std::setprecision(6) << current_M << "\n";

        if (step % 10000 == 0) {
            std::cout << "  Recorded " << step << " sweeps..." << std::endl;
        }
    }

    outFile.close();
    std::cout << "\nRaw timeseries successfully saved to: " << filename << std::endl;
    std::cout << "You can now run the Python Block Averaging script on this file." << std::endl;

    return 0;
}
*/
/*
int main() {
    // 1. The Array of Lattice Sizes to scale
    std::vector<int> L_values = {32, 64, 128, 256};

    // 2. Locked Parameters for the "Representative Slice"
    double T = 1.8;                 // Cold temperature slice
    double activeFraction = 0.95;   // 5% Quenched Defects
    double omega = 0.05 * pi;       // Resonant frequency
    double dt = 0.1;
    int period_steps = static_cast<int>((2.0 * pi) / (omega * dt));

    // Use 3-5 ensembles to smooth out localized topological variance at larger scales
    int num_ensembles = 3;

    std::ofstream outFile("data/dynamic_finite_size_scaling.csv");
    outFile << "L,Avg_Critical_H0\n";

    std::cout << "Starting Overnight Finite-Size Scaling Batch..." << std::endl;

    // THE SCALING LOOP
    for (int L : L_values) {
        std::cout << "\n=== Testing Lattice Size L = " << L << " ===" << std::endl;
        std::vector<double> ensemble_results(num_ensembles);

        // Parallelize across the independent ensembles, NOT the file-writing loop
        #pragma omp parallel for
        for (int e = 0; e < num_ensembles; ++e) {
            std::mt19937 rng(42 + e); // Thread-safe seeding
            IsingLattice lattice(L);
            lattice.initializeDefects(rng, activeFraction);

            double critical_H0 = 0.0;

            for (double H0 = 0.1; H0 <= 6.0; H0 += 0.2) {
                // Count active spins for proper macroscopic normalization
                int active_spins = 0;
                for (int i = 0; i < lattice.N; ++i) {
                    if (lattice.defects[i] == 1) active_spins++;
                }

                MonteCarlos mc(lattice, 42 + e);
                double t = 0.0;

                // Thermalization Phase
                for (int i = 0; i < 10 * period_steps; ++i) {
                    mc.metropolisSweep(T, H0, omega, t);
                    t += dt;
                }

                // Measurement Phase
                double Q_sum = 0.0;
                for (int i = 0; i < period_steps; ++i) {
                    mc.metropolisSweep(T, H0, omega, t);
                    double current_M = 0.0;
                    for (int k = 0; k < lattice.N; ++k) {
                        if (lattice.defects[k] == 1) {
                            current_M += lattice.spins[k];
                        }
                    }
                    Q_sum += (current_M / active_spins);
                    t += dt;
                }

                double Q = std::abs(Q_sum / period_steps);

                // Dynamic Disorder Threshold
                if (Q < 0.05) {
                    critical_H0 = H0;
                    break;
                }
            }

            if (critical_H0 == 0.0) critical_H0 = 6.0; // Hard magnet failsafe
            ensemble_results[e] = critical_H0;

            // Thread-safe console output so the terminal doesn't scramble
            #pragma omp critical
            {
                std::cout << "  Ensemble " << e + 1 << "/" << num_ensembles
                          << " found H_c = " << critical_H0 << std::endl;
            }
        }

        // Aggregate and save sequentially
        double sum = std::accumulate(ensemble_results.begin(), ensemble_results.end(), 0.0);
        double avg_critical_H0 = sum / num_ensembles;

        outFile << L << "," << avg_critical_H0 << "\n";
        std::cout << "-> L=" << L << " Average Critical H0: " << avg_critical_H0 << std::endl;
    }

    outFile.close();
    std::cout << "\nOvernight batch complete. Data saved to data/dynamic_finite_size_scaling.csv" << std::endl;
    return 0;
}
*/