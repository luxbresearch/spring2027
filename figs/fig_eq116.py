import matplotlib.pyplot as plt
import numpy as np
from math import sqrt

plt.rcParams.update({
    "font.family": "serif",
    "font.serif": ["Computer Modern Roman"],
})

gamma = 1.4

def eq116(M):
    # equals G√h₀/J
    return (M / (1 + gamma*M*M)) * sqrt((gamma*gamma/(gamma-1)) * (1 + (gamma-1)*M*M/2))

# Draw a plot of eq116 for M in [0, 10]
M = np.linspace(0, 10, 100)
plt.plot(M, [eq116(m) for m in M])
plt.title("Equation 1.1.6: plot of G√h₀/J vs Mach number for γ=1.4")
plt.xlabel("M")
plt.ylabel("G√h₀/J")
plt.grid(True)
plt.minorticks_on()
# Add a horizontal line at y=1/sqrt(2)

plt.axhline(y=1/sqrt(2), color='r', linestyle='--', label='G√h₀/J=1/√2')
plt.axhline(y=1.0104, color='r', linestyle='--', label='G√h₀/J=1.0104')
plt.axvline(x=1, color='r', linestyle='--', label='M=1')
plt.legend()    

plt.show()
