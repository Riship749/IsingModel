//
// Created by Rish on 5/28/2026.
//


#include "../include/IsingLattice.h"

IsingLattice::IsingLattice(int size) {
    L = size;
    N = size * size;
    spins.resize(N,1); //  default all to +1
    defects.resize(N, 1); // Default to a perfect lattice (no defects)
}
// Randomize the lattice for the high-temperature starting point
void IsingLattice::initializeRandom(std::mt19937 &rng) {
    std::uniform_int_distribution<> dist(0, 1);
    for (int i = 0; i < N; ++i) {
        spins[i] = dist(rng) == 0 ? -1 : 1;
    }
}

// Set all spins to a specific value (useful for T=0 checks or testing)
void IsingLattice::initializeHomogeneous(int spinValue) {
    for (int i = 0; i < N; ++i) {
        spins[i] = spinValue;
    }
}

// Safely get a spin, utilizing the PBC wrapping logic
int IsingLattice::getSpin(int x, int y) const {
    return spins[getIndex(x, y)];
}

// Flip a specific spin
void IsingLattice::flipSpin(int x, int y) {
    spins[getIndex(x, y)] *= -1;
}
//getter for defects
int IsingLattice::getDefect(int x, int y) const {
    return defects[getIndex(x, y)];
}
//randonally get defects
void IsingLattice::initializeDefects(std::mt19937 &rng, double activeFraction) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < N; ++i) {
        // If the random number is greater than the active fraction, it becomes a defect (0)
        defects[i] = (dist(rng) < activeFraction) ? 1 : 0;
    }
}

// Calculate the energy change IF the spin at (x,y) were to flip
int IsingLattice::calculateDeltaE(int x, int y, double H_ext, int J) const {
    int idx = getIndex(x, y);

    // Fast-fail: If this site is a defect, it can't flip, energy change is 0
    if (defects[idx] == 0) return 0.0;

    int currentSpin = spins[idx];

    // Sum of the 4 nearest neighbors, masked by whether they are defects
    int neighborSum = (getSpin(x + 1, y) * getDefect(x + 1, y)) +
                      (getSpin(x - 1, y) * getDefect(x - 1, y)) +
                      (getSpin(x, y + 1) * getDefect(x, y + 1)) +
                      (getSpin(x, y - 1) * getDefect(x, y - 1));

    // Delta E = 2 * currentSpin * (J * neighborSum + H_ext)
    return 2.0 * currentSpin * ((J * neighborSum) + H_ext);
}

// Calculate the total magnetization (sum of all spins)
int IsingLattice::calculateTotalMagnetization() const {
    int M = 0;
    for (int i = 0; i < N; ++i) {
        M += spins[i] * defects[i];
    }
    return M;
}

// Calculate the total energy of the entire lattice
int IsingLattice::calculateTotalEnergy(int J) const {
    int E = 0;
    for (int y = 0; y < L; ++y) {
        for (int x = 0; x < L; ++x) {
            int currentSpin = getSpin(x, y);
            // To avoid double-counting interactions, we only look Forward and Down
            int rightSpin = getSpin(x + 1, y);
            int downSpin = getSpin(x, y + 1);

            E -= J * currentSpin * (rightSpin + downSpin);
        }
    }
    return E;
}