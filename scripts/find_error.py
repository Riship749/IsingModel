import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
# Load the new CSV
df = pd.read_csv('../data/phase3_final_map.csv')


plt.figure(figsize=(10, 6))

# Plot each defect fraction
colors = {0.0: 'navy', 0.1: 'forestgreen', 0.2: 'goldenrod', 0.3: 'firebrick'}
labels = {0.0: '0% Defects (Pure)', 0.1: '10% Defects', 0.2: '20% Defects', 0.3: '30% Defects'}

for defect_frac in [0.0, 0.1, 0.2, 0.3]:
    subset = df[abs(df['DefectFraction'] - defect_frac) < 0.01]
    plt.plot(subset['Temperature'], subset['Avg_Critical_H0'],
             label=labels[defect_frac], color=colors[defect_frac], linewidth=2.5)

plt.title('Dynamic Phase Boundaries: The Kinetic Bottleneck Effect', fontsize=14, pad=15)
plt.xlabel('Temperature (T)', fontsize=12)
plt.ylabel('Critical Field Amplitude ($H_{0,c}$)', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.6)
plt.legend(fontsize=11)

# Set axes to show the massive increase in H0
plt.xlim(0.2, 3.0)
plt.ylim(0, 6.5)

plt.tight_layout()
plt.savefig('Figure_3_New.png', dpi=300)
print("Figure 3 successfully generated!")