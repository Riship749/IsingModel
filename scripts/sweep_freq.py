import pandas as pd
import matplotlib.pyplot as plt

# The frequency labels we ran
omega_labels = ["0.01", "0.05", "0.10", "0.20"]
colors = ['blue', 'green', 'red', 'yellow']

plt.figure(figsize=(10, 7))

for label, color in zip(omega_labels, colors):
    filename = f'../data/dynamic_boundary_w{label}.csv'
    df = pd.read_csv(filename)

    # Pivot into a 2D grid
    grid = df.pivot(index='H0', columns='Temperature', values='Q_OrderParameter')

    # Draw the boundary line exactly where Q drops to near-zero
    contour = plt.contour(grid.columns, grid.index, grid.values, levels=[0.05], colors=[color], linewidths=2.5)

    # Custom legend entry
    plt.plot([], [], color=color, linewidth=2.5, label=f'$\omega = {label}\pi$')

plt.title('Dynamic Phase Boundaries: The Effect of Driving Frequency', fontsize=14)
plt.xlabel('Temperature ($T$)', fontsize=12)
plt.ylabel('External Field Amplitude ($H_0$)', fontsize=12)
plt.legend(loc='upper right')
plt.grid(True, linestyle='--', alpha=0.5)

# Add a text box to remind the reader of the spatial constant
plt.text(0.6, 0.2, 'Constant Quenched Defects: 10%', fontsize=10,
         bbox=dict(facecolor='white', alpha=0.8, edgecolor='black'))

plt.savefig('../data/frequency_scaling_overlay.png', dpi=300)
plt.show()