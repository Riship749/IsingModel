#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <sstream>
#include <numeric>
#include "include/IsingLattice.h"
#include "include/MonteCarlos.h"

const double pi = std::acos(-1.0);

// EXPERIMENT 1: Frequency Sweep (Figure 5)
void runFrequencySweep() {
    int L = 32;

    // LOCK IN SPACE: Fix the lattice at 10% Quenched Defects
    double activeFraction = 0.90;

    // A list of the driving frequencies (omega) we want to test
    std::vector<double> omegas = {0.01 * pi, 0.05 * pi, 0.10 * pi, 0.20 * pi};
    std::vector<std::string> omega_labels = {"0.01", "0.05", "0.10", "0.20"};

    int stepsPerPeriod = 200;
    int transientPeriods = 50;
    int dataPeriods = 20;

    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Automated Frequency Scaling Batch..." << std::endl;

    for (size_t i = 0; i < omegas.size(); ++i) {
        double omega = omegas[i];
        std::string filename = "data/dynamic_boundary_w" + omega_labels[i] + ".csv";

        std::ofstream outFile(filename);
        outFile << "Temperature,H0,Q_OrderParameter\n";

        std::cout << "\n=== Running Frequency omega = " << omega_labels[i] << "*pi ===" << std::endl;

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
                        << T << "," << H0 << "," << absQ << "\n";
            }
        }
        outFile.close();
        std::cout << "Saved: " << filename << std::endl;
    }

    std::cout << "\nAll frequency simulations complete!" << std::endl;
}


// EXPERIMENT 2: Raw Timeseries / Error Bars
void extractRawTimeseries() {
    int L = 32;

    // Lock parameters to the critical point for baseline error analysis
    double T = 2.27;
    double H0 = 0.0;

    int totalSweeps = 100000;
    int thermalizationSweeps = 10000;

    std::mt19937 rng(42);

    std::string filename = "data/raw_timeseries.csv";
    std::ofstream outFile(filename);
    outFile << "Sweep,Magnetization\n";

    std::cout << "Starting Raw Timeseries Extraction..." << std::endl;
    std::cout << "Parameters: L=" << L << ", T=" << T << ", H0=" << H0 << std::endl;

    IsingLattice lattice(L);
    lattice.initializeDefects(rng, 1.0); // Pure lattice validation
    lattice.initializeRandom(rng);

    MonteCarlos mc(lattice, 42);

    std::cout << "Thermalizing for " << thermalizationSweeps << " sweeps..." << std::endl;
    for (int i = 0; i < thermalizationSweeps; ++i) {
        mc.metropolisSweep(T, H0, 0.0);
    }

    std::cout << "Collecting raw data for " << totalSweeps << " sweeps..." << std::endl;
    for (int step = 1; step <= totalSweeps; ++step) {
        mc.metropolisSweep(T, H0, 0.0);

        // Extract the absolute macroscopic magnetization
        double current_M = std::abs(lattice.calculateTotalMagnetization());

        outFile << step << "," << std::fixed << std::setprecision(6) << current_M << "\n";

        if (step % 10000 == 0) {
            std::cout << "  Recorded " << step << " sweeps..." << std::endl;
        }
    }

    outFile.close();
    std::cout << "\nRaw timeseries successfully saved to: " << filename << std::endl;
}

// EXPERIMENT 3: Dynamic Phase Boundary Heatmap
void mapPhaseBoundary() {
    int L = 32;
    double activeFraction = 0.90;

    double omega = 2.0 * pi * 0.01;
    int stepsPerPeriod = 200;
    int transientPeriods = 50;
    int dataPeriods = 20;

    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Dynamic Phase Transition Mapping with Quenched Defects..." << std::endl;

    std::ofstream outFile("data/dynamic_boundary.csv");
    outFile << "Temperature,H0,Q_OrderParameter\n";

    IsingLattice lattice(L);
    lattice.initializeDefects(rng, activeFraction);

    for (double H0 = hStart; H0 <= hEnd; H0 += hStep) {
        std::cout << "Simulating Field Amplitude H0 = " << std::fixed << std::setprecision(1) << H0 << "..." << std::endl;

        lattice.initializeRandom(rng);
        MonteCarlos mc(lattice, 42);

        for (double T = tStart; T >= tEnd; T -= tStep) {
            DynamicObservables obs = mc.simulateDynamic(T, H0, omega, dataPeriods, stepsPerPeriod, transientPeriods);
            double absQ = std::abs(obs.Q);

            outFile << std::fixed << std::setprecision(4)
                    << T << "," << H0 << "," << absQ << "\n";
        }
    }

    outFile.close();
    std::cout << "Mapping complete. Data saved to data/dynamic_boundary.csv" << std::endl;
}


// EXPERIMENT 4: Defect Scaling (0% to 30%)

void runDefectSweep() {
    int L = 32;
    std::vector<double> activeFractions = {1.0, 0.90, 0.80, 0.70};

    double omega = 2.0 * pi * 0.01;
    int stepsPerPeriod = 200;
    int transientPeriods = 50;
    int dataPeriods = 20;

    double tStart = 3.0, tEnd = 0.5, tStep = 0.1;
    double hStart = 0.1, hEnd = 2.0, hStep = 0.1;

    std::mt19937 rng(42);

    std::cout << "Starting Automated Defect Scaling Batch..." << std::endl;

    for (double activeFraction : activeFractions) {
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
                        << T << "," << H0 << "," << absQ << "\n";
            }
        }
        outFile.close();
        std::cout << "Saved: " << filename << std::endl;
    }

    std::cout << "\nAll batch simulations complete!" << std::endl;
}


// EXPERIMENT 5: Overnight Finite-Size Scaling
void runFiniteSizeScaling() {
    std::vector<int> L_values = {32, 64, 128, 256};

    double T = 1.8;
    double activeFraction = 0.95;
    double omega = 0.05 * pi;
    double dt = 0.1;
    int period_steps = static_cast<int>((2.0 * pi) / (omega * dt));

    int num_ensembles = 3;

    std::ofstream outFile("data/dynamic_finite_size_scaling.csv");
    outFile << "L,Avg_Critical_H0\n";

    std::cout << "Starting Overnight Finite-Size Scaling Batch..." << std::endl;

    for (int L : L_values) {
        std::cout << "\n=== Testing Lattice Size L = " << L << " ===" << std::endl;
        std::vector<double> ensemble_results(num_ensembles);

        #pragma omp parallel for
        for (int e = 0; e < num_ensembles; ++e) {
            std::mt19937 rng(42 + e);
            IsingLattice lattice(L);
            lattice.initializeDefects(rng, activeFraction);

            double critical_H0 = 0.0;

            for (double H0 = 0.1; H0 <= 6.0; H0 += 0.2) {
                int active_spins = 0;
                for (int i = 0; i < lattice.N; ++i) {
                    if (lattice.defects[i] == 1) active_spins++;
                }

                MonteCarlos mc(lattice, 42 + e);
                double t = 0.0;

                for (int i = 0; i < 10 * period_steps; ++i) {
                    mc.metropolisSweep(T, H0, omega, t);
                    t += dt;
                }

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

                if (Q < 0.05) {
                    critical_H0 = H0;
                    break;
                }
            }

            if (critical_H0 == 0.0) critical_H0 = 6.0;
            ensemble_results[e] = critical_H0;

            #pragma omp critical
            {
                std::cout << "  Ensemble " << e + 1 << "/" << num_ensembles
                          << " found H_c = " << critical_H0 << std::endl;
            }
        }

        double sum = std::accumulate(ensemble_results.begin(), ensemble_results.end(), 0.0);
        double avg_critical_H0 = sum / num_ensembles;

        outFile << L << "," << avg_critical_H0 << "\n";
        std::cout << "-> L=" << L << " Average Critical H0: " << avg_critical_H0 << std::endl;
    }

    outFile.close();
    std::cout << "\nOvernight batch complete. Data saved to data/dynamic_finite_size_scaling.csv" << std::endl;
}


// MAIN EXECUTION HUB

int main(int argc, char* argv[]) {
    // If the user doesn't provide an argument, show them the options
    if (argc < 2) {
        std::cout << "Usage: ./ising_sim [experiment_name]\n\n"
                  << "Available Experiments:\n"
                  << "  freq       - Run the frequency sweep (Fig 5)\n"
                  << "  timeseries - Extract raw timeseries for error analysis\n"
                  << "  heatmap    - Map the dynamic phase boundary\n"
                  << "  defect     - Run the 0% to 30% defect concentration sweep\n"
                  << "  scaling    - Run overnight finite size scaling\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "freq") {
        runFrequencySweep();
    } else if (mode == "timeseries") {
        extractRawTimeseries();
    } else if (mode == "heatmap") {
        mapPhaseBoundary();
    } else if (mode == "defect") {
        runDefectSweep();
    } else if (mode == "scaling") {
        runFiniteSizeScaling();
    } else {
        std::cerr << "Error: Unknown experiment '" << mode << "'\n";
        return 1;
    }

    return 0;
}