import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import linregress
import os

# Define the lattice sizes you simulated
lattice_sizes = [2, 4, 8, 16, 32]
data_dir = "../data/"

# Set up a 2x2 grid of plots for the macroscopic observables
fig, axs = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle("2D Ising Model Monte Carlo Simulation Results", fontsize=16)

# Lists to hold peak values for finite-size scaling later
L_vals = []
chi_max_vals = []

# Loop through each lattice size and plot the data
for L in lattice_sizes:
    file_path = os.path.join(data_dir, f"results_L{L}.csv")

    try:
        df = pd.read_csv(file_path)
    except FileNotFoundError:
        print(f"Could not find {file_path}. Skipping.")
        continue

    # Plot Energy vs Temperature
    axs[0, 0].plot(df['Temperature'], df['Energy'], marker='.', linestyle='-', label=f'L={L}')

    # Plot Absolute Magnetization vs Temperature
    axs[0, 1].plot(df['Temperature'], df['Magnetization'], marker='.', linestyle='-', label=f'L={L}')

    # Plot Heat Capacity vs Temperature
    axs[1, 0].plot(df['Temperature'], df['HeatCapacity'], marker='.', linestyle='-', label=f'L={L}')

    # Plot Susceptibility vs Temperature
    axs[1, 1].plot(df['Temperature'], df['Susceptibility'], marker='.', linestyle='-', label=f'L={L}')

    # Extract max susceptibility for Finite Size Scaling
    max_chi = df['Susceptibility'].max()
    L_vals.append(L)
    chi_max_vals.append(max_chi)

# Formatting the subplots
axs[0, 0].set(xlabel='Temperature (T)', ylabel='Energy per spin (E/N)', title='Energy')
axs[0, 1].set(xlabel='Temperature (T)', ylabel='Magnetization per spin (|M|/N)', title='Absolute Magnetization')
axs[1, 0].set(xlabel='Temperature (T)', ylabel='Specific Heat Capacity (C/N)', title='Heat Capacity')
axs[1, 1].set(xlabel='Temperature (T)', ylabel='Magnetic Susceptibility ($\chi$/N)', title='Susceptibility')

for ax in axs.flat:
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.invert_xaxis() # Optional: Matches the paper's convention if they plotted low to high

plt.tight_layout()
plt.subplots_adjust(top=0.92)
plt.savefig("../data/observables_plot.png", dpi=300)
print("Saved observables plot to data/observables_plot.png")

# Step 2: Finite Size Scaling (Log-Log Plot for Susceptibility)
if len(L_vals) > 1:
    plt.figure(figsize=(8, 6))

    # Take the natural log of L and Chi_max
    ln_L = np.log(L_vals)
    ln_chi = np.log(chi_max_vals)

    # Perform a linear regression: ln(Chi) = (gamma/nu) * ln(L) + constant
    slope, intercept, r_value, p_value, std_err = linregress(ln_L, ln_chi)

    # Plot the data points and the line of best fit
    plt.scatter(ln_L, ln_chi, color='black', label='Data points')
    plt.plot(ln_L, intercept + slope * ln_L, 'r--', label=f'Fit: slope = {slope:.3f}')

    plt.xlabel('ln(L)')
    plt.ylabel('ln($\chi_{max}$)')
    plt.title('Finite Size Scaling: Susceptibility')
    plt.legend()
    plt.grid(True, alpha=0.3)

    plt.savefig("../data/scaling_plot_2M.png", dpi=300)
    print("Saved scaling plot to data/scaling_plot_2M.png")

    print("\n--- Finite Size Scaling Results ---")
    print(f"Calculated Critical Exponent (gamma / nu): {slope:.4f}")
    print(f"Theoretical Value for 2D Ising Model: 1.7500")
    print(f"R-squared of fit: {r_value**2:.4f}")

plt.show()