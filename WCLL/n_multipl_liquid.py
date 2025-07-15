import os
import matplotlib.pyplot as plt
import numpy as np

# Set default font sizes for consistency
plt.rcParams.update({
    'axes.titlesize': 16,
    'axes.labelsize': 14,
    'xtick.labelsize': 14,
    'ytick.labelsize': 14,
    'legend.fontsize': 12
})

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
                    z = float(parts[1])  # raw global z-position in cm
                    depths.append(z)
            except ValueError:
                continue
    return depths

# Load and shift data
depths = read_multiplication_depths(mult_file)
if not depths:
    print("Nothing to plot. File is empty or missing.")
    exit()

depth_offset = 0  # Shift so that z = -52.0 cm → depth = 0 cm
depths = [z + depth_offset for z in depths]
print(f"Shifted raw Z positions by {depth_offset:+.1f} cm to align 0 with vacuum–W interface (z = -52.0 cm)")

# Histogram bins
min_depth = min(depths)
max_depth = max(depths)
num_bins = 120
bins = np.linspace(min_depth, max_depth, num_bins + 1)

# Plot
fig, ax = plt.subplots(figsize=(10, 6))
ax.hist(depths, bins=bins, color='blue', alpha=0.7,
        edgecolor='black', linewidth=0.5)

# Set log scale and manual y-axis limit
ax.set_yscale('log')
ax.set_ylim(top=1e6)

# Material regions and annotations
label_y = 1e6 * 0.4  # Vertical position for layer labels

# W layer
ax.axvspan(0, 2.0, facecolor='gray', alpha=0.2)
ax.text(1.0, label_y, 'W', ha='center', fontsize=12)

# Li₁₇Pb₈₃ breeder layer
ax.axvspan(2.0, 82.0, facecolor='orange', alpha=0.15)
ax.text(42.0, label_y, 'Li₁₇Pb₈₃', ha='center', fontsize=12)

# EUROFER layer
ax.axvspan(82.0, 97.0, facecolor='red', alpha=0.1)
ax.text(89.5, label_y, 'EUROFER', ha='center', fontsize=12)

# Dotted vertical lines at region boundaries
for x in [0, 2.0, 82.0, 97.0]:
    ax.axvline(x, color='black', linestyle='--')

# Labels and formatting
ax.set_title('Neutron Multiplication Depth (Li₁₇Pb₈₃ Geometry)', fontsize=16)
ax.set_xlabel('Depth from Geometry Entry Interface (cm)', fontsize=14)
ax.set_ylabel('Multiplication Counts', fontsize=14)
ax.grid(True)
ax.tick_params(axis='both', labelsize=14)

plt.tight_layout(rect=[0, 0, 1, 0.95])
plt.show()
