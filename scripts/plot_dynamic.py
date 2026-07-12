import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


# Load the dynamic boundary data
df = pd.read_csv('../data/dynamic_boundary.csv')

# Pivot the data into a proper 2D matrix for the heatmap
grid = df.pivot(index='H0', columns='Temperature', values='Q_OrderParameter')

plt.figure(figsize=(10, 7))

# pcolormesh is much cleaner and more accurate for discrete grid data than tricontourf
plt.pcolormesh(grid.columns, grid.index, grid.values, shading='auto', cmap='plasma')

plt.colorbar(label='Dynamic Order Parameter (|Q|)')
plt.xlabel('Temperature (T)')
plt.ylabel('Field Amplitude ($H_0$)')
plt.title('Dynamic Phase Boundary with 10% Quenched Defects')

plt.savefig('../data/dynamic_heatmap_clean.png', dpi=300)
plt.show()