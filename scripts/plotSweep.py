import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('../data/phase3_defects_ensemble.csv')

# Group the data by defect fraction
for fraction, group in df.groupby('DefectFraction'):

    # Optional: Format label to say "10% Defects"
    label_name = f"{int(fraction * 100)}% Defects"

    # Plot the main average line
    plt.plot(group['Temperature'], group['Avg_Critical_H0'], label=label_name)

    # Shade the standard deviation region
    plt.fill_between(
        group['Temperature'],
        group['Avg_Critical_H0'] - group['Std_Dev_H0'],
        group['Avg_Critical_H0'] + group['Std_Dev_H0'],
        alpha=0.2 # Makes it 20% transparent so you can see overlapping boundaries
    )

plt.xlabel('Temperature (T)')
plt.ylabel('External Field Amplitude ($H_0$)')
plt.title('Dynamic Phase Boundaries: The Effect of Domain Wall Pinning')
plt.legend()
plt.grid(True, linestyle='--', alpha=0.7)
plt.show()