import pandas as pd
import matplotlib.pyplot as plt

# The defect percentages you ran
defect_pcts = [0, 10, 20, 30]
colors = ['blue', 'green', 'orange', 'red']

plt.figure(figsize=(10, 7))

for pct, color in zip(defect_pcts, colors):
    filename = f'../data/dynamic_boundary_{pct}pct.csv'
    df = pd.read_csv(filename)

    # Pivot into a 2D grid
    grid = df.pivot(index='H0', columns='Temperature', values='Q_OrderParameter')

    # 0.05 is a good threshold to separate the noise of the disordered phase from the ordered phase
    contour = plt.contour(grid.columns, grid.index, grid.values, levels=[0.05], colors=[color], linewidths=2.5)

    # Add a custom legend entry using a proxy artist
    plt.plot([], [], color=color, linewidth=2.5, label=f'{pct}% Defects')

plt.title('Dynamic Phase Boundaries: The Effect of Domain Wall Pinning', fontsize=14)
plt.xlabel('Temperature ($T$)', fontsize=12)
plt.ylabel('External Field Amplitude ($H_0$)', fontsize=12)
plt.legend(loc='upper right')
plt.grid(True, linestyle='--', alpha=0.5)

plt.savefig('../data/defect_scaling_overlay.png', dpi=300)
plt.show()