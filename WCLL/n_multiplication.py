import os
import matplotlib.pyplot as plt
import numpy as np

# File path
script_dir = os.path.dirname(os.path.abspath(__file__))
mult_file = os.path.join(script_dir, 'neutron_multiplication_depth.txt')

# Read depth data (ignoring material name)
def read_multiplication_depths(filepath):
    depths = []
    if not os.path.exists(filepath):
        print(f"Warning: File '{os.path.basename(filepath)}' not found.")
        return depths
    with open(filepath, 'r') as file:
        for line in file:
            try:
                parts = line.strip().split()
                if len(parts) >= 2:
                    z = float(parts[1])
                    depths.append(z)
            except ValueError:
                continue
    return depths

# Load data
multiplication_depths = read_multiplication_depths(mult_file)

if not multiplication_depths:
    print("Nothing to plot. File is empty or missing.")
    exit()

# Histogram setup
min_depth = min(multiplication_depths)
max_depth = max(multiplication_depths)
bins = np.linspace(min_depth, max_depth, 121)  # 120 bins

# Material regions — updated for new geometry
regions = {
    "W":       (0.0, 2.0),
    "Plate2":  (2.0, 82.0),     # Interleaved breeder
    "EUROFER": (82.0, 97.0),
}

region_colors = {
    "W": 'lightgrey',
    "Plate2": 'lavenderblush',
    "EUROFER": 'lightsteelblue',
}

# Plot
fig, ax = plt.subplots(figsize=(10, 5))
ax.hist(multiplication_depths, bins=bins, color='blue', alpha=0.7,
        edgecolor='black', linewidth=0.5)

# Shade material regions and draw interface lines
ylim = ax.get_ylim()
for material, (start, end) in regions.items():
    ax.axvspan(start, end, color=region_colors[material], alpha=0.3)
    ax.text((start + end) / 2, ylim[1] * 0.9, material,
            ha='center', va='top', fontsize=9, color='black', alpha=0.8)

for x in [2.0, 82.0, 97.0]:
    ax.axvline(x=x, color='black', linestyle='dotted', linewidth=1)

# Labels and formatting
ax.set_title('Neutron Multiplication Depth', fontsize=16)
ax.set_xlabel('Depth from Geometry Entry Interface (cm)')
ax.set_ylabel('Multiplication Counts')
ax.grid(True)
plt.tight_layout()
plt.show()
