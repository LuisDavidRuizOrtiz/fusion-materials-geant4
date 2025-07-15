import os
import matplotlib.pyplot as plt
import numpy as np

# Get the directory where the script is located
script_dir = os.path.dirname(os.path.abspath(__file__))

# Filenames
before_file         = os.path.join(script_dir, 'neutrons_before_W.txt')
after_w_file        = os.path.join(script_dir, 'neutrons_after_W.txt')
before_eurofer_file = os.path.join(script_dir, 'neutrons_before_EUROFER.txt')
after_eurofer_file  = os.path.join(script_dir, 'neutrons_after_EUROFER.txt')

# Function to read energy data from file
def read_energies(filepath):
    energies = []
    if not os.path.exists(filepath):
        print(f"Warning: File '{os.path.basename(filepath)}' not found.")
        return energies
    with open(filepath, 'r') as file:
        for line in file:
            try:
                energy = float(line.strip())
                energies.append(energy)
            except ValueError:
                continue  # Skip malformed lines
    return energies

# Read data
energies_before         = read_energies(before_file)
energies_after_w        = read_energies(after_w_file)
energies_before_eurofer = read_energies(before_eurofer_file)
energies_after_eurofer  = read_energies(after_eurofer_file)

# Check if there's anything to plot
if not (energies_before or energies_after_w or energies_before_eurofer or energies_after_eurofer):
    print("Nothing to plot. All files are empty or missing.")
    exit()

# Combine all energies to compute common bin edges
all_energies = energies_before + energies_after_w + energies_before_eurofer + energies_after_eurofer
min_energy = min(all_energies)
max_energy = max(all_energies)

# Define common bin edges
num_bins = 130
bins = np.linspace(min_energy, max_energy, num_bins + 1)

# Plotting
plt.figure(figsize=(10, 6))

if energies_before:
    plt.hist(energies_before, bins=bins, alpha=0.5, label='Before W', edgecolor='black', linewidth=0.5)
if energies_after_w:
    plt.hist(energies_after_w, bins=bins, alpha=0.5, label='After W', edgecolor='black', linewidth=0.5)
if energies_before_eurofer:
    plt.hist(energies_before_eurofer, bins=bins, alpha=0.5, label='Before EUROFER', edgecolor='black', linewidth=0.5)
if energies_after_eurofer:
    plt.hist(energies_after_eurofer, bins=bins, alpha=0.5, label='After EUROFER', edgecolor='black', linewidth=0.5)

plt.tick_params(axis='both', which='major', labelsize=12)
plt.xlabel('Neutron Energy (MeV)', fontsize=14)
plt.ylabel('Counts', fontsize=14)
plt.title('Diseño WCLL: Blanket de aleación líquida $\mathrm{Pb_{83}Li{17}}$')
plt.legend(fontsize=12)
plt.grid()
plt.tight_layout()

# Optional: Log scale for y-axis
plt.yscale('log')

# Optional: Save figure
# plt.savefig(os.path.join(script_dir, 'neutron_spectra.png'))

plt.show()
