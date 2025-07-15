import matplotlib.pyplot as plt

# Datos de energía depositada por capa (en GeV)
energia = [694.051, 3931.41, 539.019, 1617.02, 156.313,
           423.242, 36.4291, 92.872, 8.16043, 19.1231]

# Material por capa (alternancia)
materiales = ['Be', 'Li2TiO3'] * 5
capas = list(range(1, 11))  # Índices de capa (1 a 10)

plt.figure(figsize=(8, 5))

# Dibujar barras
for i, (capa, energia_val, mat) in enumerate(zip(capas, energia, materiales)):
    color = 'steelblue' if mat == 'Be' else 'orange'
    label = 'Be' if mat == 'Be' and i == 0 else ('Li$_2$TiO$_3$' if mat != 'Be' and i == 1 else None)
    plt.bar(capa, energia_val, color=color, label=label, width=0.6)

# Escala logarítmica y etiquetas
plt.yscale('log')
plt.xticks(capas, fontsize=18)
plt.yticks(fontsize=12)
plt.xlabel('Capa', fontsize=18)
plt.ylabel('Energía depositada (GeV)', fontsize=18)
#plt.title('Distribución de energía depositada en capas del blanket cerámico (escala logarítmica)', fontsize=16)
plt.legend(fontsize=18)
plt.grid(axis='y', linestyle='--', alpha=0.6, which='both')

# Ajuste: estrechar rango del eje X
plt.xlim(0.5, 10.5)  # Con esto las barras se ven más juntas

plt.tight_layout()
plt.show()
