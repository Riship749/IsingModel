//
// Created by Rish on 5/28/2026.
//

#ifndef ISINGMODEL_ISINGLATTICE_H
#define ISINGMODEL_ISINGLATTICE_H


#pragma  once
#include  "iostream"
#include <random>
#include <vector>
class IsingLattice {
private:
    int L; // latice dimesntion (l by L)
    int N; // Number of spins
    std::vector<int> spins;
    std::vector<int> defects;

    inline int getIndex(int x, int y) const {
        // We add L before modulo to handle negative values safely in C++
        int safeX = (x % L + L) % L;
        int safeY = (y % L + L) % L;
        return safeY * L + safeX;
    }
public:
    IsingLattice(int size);
    void initializeRandom(std::mt19937& rng);
    void initializeHomogeneous(int spinValue);
    void initializeDefects(std::mt19937& rng, double activeFraction); // NEW
    // Spin operations
    int getSpin(int x, int y) const;
    void flipSpin(int x, int y);
    int getDefect(int x, int y) const; // NEW

    // Physics calculations
    int calculateDeltaE(int x, int y, double H_ext, int J = 1) const;
    int calculateTotalMagnetization() const;
    int calculateTotalEnergy(int J = 1) const;

    // Getters
    int getSize() const { return L; }
    int getNumSpins() const { return N; }

};

#endif //ISINGMODEL_ISINGLATTICE_H
