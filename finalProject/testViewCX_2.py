import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

df = pd.read_csv('finalProject/cnot_data_hadamard.csv')

fig, ax = plt.subplots()
states = ['|00>', '|01>', '|10>', '|11>']

def animate(i):
    ax.clear()
    row = df.iloc[i]
    probs = [row['p00'], row['p01'], row['p10'], row['p11']]
    ax.bar(states, probs, color=['blue', 'blue', 'orange', 'green'])
    ax.set_ylim(0, 1)
    ax.set_title(f"Time: {row['time']:.2f}s - CNOT Pulse in Action")
    ax.set_ylabel("Probability")

ani = animation.FuncAnimation(fig, animate, frames=len(df), interval=20)

# Save the animation as a GIF
ani.save('finalProject/cnot_animation_2.gif', writer='pillow', fps=50)

plt.show()