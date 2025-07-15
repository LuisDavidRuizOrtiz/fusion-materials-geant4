import os
import matplotlib.pyplot as plt
import numpy as np

# Set default font sizes for better readability
plt.rcParams.update({
    'axes.titlesize': 16,
    'axes.labelsize': 14,
    'xtick.labelsize': 14,
    'ytick.labelsize': 14,
    'legend.fontsize': 12
})

# Get the directory where this script is located
script_dir = os.path.dirname(os.path.abspath(__file__))

# File paths
triton_file = os.path.join(script_dir, 'triton_depth.txt')
alpha_file  = os.path.join(script_dir, 'alpha_depth.txt')

# Function to read raw Z-position data (in cm)
def read_depths(filepath):
    depths = []
    if not os.path.exists(filepath):
        print(f"Warning: File '{os.path.basename(filepath)}' not found.")
        return depths
    with open(filepath, 'r') as file:
        for line in file:
            try:
                z = float(line.strip())  # raw global z-position in cm
                depths.append(z)
            except ValueError:
                continue
    return depths

# Read raw global Z-positions
triton_depths = read_depths(triton_file)
alpha_depths  = read_depths(alpha_file)

# Shift so that z = -52.0 cm becomes depth = 0 cm
depth_offset = +29.5  # cm
triton_depths = [z + depth_offset for z in triton_depths]
alpha_depths  = [z + depth_offset for z in alpha_depths]

print(f"Shifted raw Z positions by {depth_offset:+.1f} cm to align 0 with vacuum–W interface (z = -52.0 cm)")

# Check data
if not (triton_depths or alpha_depths):
    print("Nothing to plot. Both files are empty or missing.")
    exit()

# Determine common bins
all_depths = triton_depths + alpha_depths
min_depth = min(all_depths)
max_depth = max(all_depths)
num_bins = 120
bins = np.linspace(min_depth, max_depth, num_bins + 1)

# Create stacked subplots with shared x-axis
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(10, 8), height_ratios=[1, 1])

# Tritium (top)
if triton_depths:
    ax1.hist(triton_depths, bins=bins, alpha=0.7, color='green', edgecolor='black', linewidth=0.5)
    ax1.set_ylabel('Tritium Counts', fontsize=16)
    ax1.set_title('Tritium Production Depth', fontsize=16)
    ax1.grid(True)
    ax1.tick_params(axis='both', labelsize=16)

# Helium (bottom)
if alpha_depths:
    ax2.hist(alpha_depths, bins=bins, alpha=0.7, color='purple', edgecolor='black', linewidth=0.5)
    ax2.set_ylabel('Helium Counts', fontsize=16)
    ax2.set_title('Helium Production Depth', fontsize=16)
    ax2.set_xlabel('Depth from Geometry Entry Interface (cm)', fontsize=14)
    ax2.grid(True)
    ax2.tick_params(axis='both', labelsize=16)

# Annotate material regions and transitions
for ax in [ax1, ax2]:
    # W plate
    ax.axvspan(0, 2.0, facecolor='gray', alpha=0.2)
    ax.text(1.0, ax.get_ylim()[1] * 0.7, 'W', ha='center', fontsize=12)

    # Breeder layers
    be_thickness = 6.4
    li_thickness = 9.6
    z_cursor = 2.0
    breeder_edges = [z_cursor]  # start at 2.0 cm

    for i in range(10):
        is_be = (i % 2 == 0)
        thick = be_thickness if is_be else li_thickness
        color = 'yellow' if is_be else 'cyan'
        label = 'Be' if is_be else 'Li₂TiO₃'

        ax.axvspan(z_cursor, z_cursor + thick, facecolor=color, alpha=0.15)
        ax.text(z_cursor + thick / 2, ax.get_ylim()[1] * 0.7, label, ha='center', fontsize=12)

        z_cursor += thick
        breeder_edges.append(z_cursor)

    # EUROFER
    ax.axvspan(82.0, 97.0, facecolor='red', alpha=0.1)
    ax.text(89.5, ax.get_ylim()[1] * 0.7, 'EUROFER', ha='center', fontsize=12)

    # Dotted vertical lines at all region transitions (including breeder interfaces)
    for x in [0, 2.0, 82.0, 97.0] + breeder_edges[1:-1]:
        ax.axvline(x, color='black', linestyle='--')

# Final layout
plt.tight_layout(rect=[0, 0, 1, 0.95])
ax1.set_yscale('log')
ax2.set_yscale('log')
plt.show()
