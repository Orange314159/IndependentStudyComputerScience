import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def plot_bloch_spheres(csv_file):
    # Load the simulation data
    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: {csv_file} not found.")
        return

    time = df['time'].values
    
    # Identify qubit count based on column naming convention (qx_x, qx_y, qx_z)
    qubit_cols = [col for col in df.columns if col.endswith('_x')]
    num_qubits = len(qubit_cols)
    
    # Calculate grid dimensions
    cols = min(num_qubits, 3)
    rows = (num_qubits + cols - 1) // cols
    
    fig = plt.figure(figsize=(5 * cols, 5 * rows))
    
    for i in range(num_qubits):
        ax = fig.add_subplot(rows, cols, i + 1, projection='3d')
        
        # 1. Draw the Bloch Sphere wireframe
        u = np.linspace(0, 2 * np.pi, 20)
        v = np.linspace(0, np.pi, 20)
        x_sphere = np.outer(np.cos(u), np.sin(v))
        y_sphere = np.outer(np.sin(u), np.sin(v))
        z_sphere = np.outer(np.ones(np.size(u)), np.cos(v))
        ax.plot_wireframe(x_sphere, y_sphere, z_sphere, color="k", alpha=0.05, linewidth=0.5)
        
        # 2. Draw the X, Y, Z axes lines
        span = np.array([-1.1, 1.1])
        zeros = np.zeros(2)
        ax.plot(span, zeros, zeros, color='black', alpha=0.3, ls='--') 
        ax.plot(zeros, span, zeros, color='black', alpha=0.3, ls='--') 
        ax.plot(zeros, zeros, span, color='black', alpha=0.3, ls='--') 

        # 3. Plot the data
        qx = df[f'q{i}_x'].values
        qy = df[f'q{i}_y'].values
        qz = df[f'q{i}_z'].values
        
        # Scatter plot with time-based colormap
        p = ax.scatter(qx, qy, qz, c=time, cmap='viridis', s=2)
        
        # Plot the final state vector as an arrow
        ax.quiver(0, 0, 0, qx[-1], qy[-1], qz[-1], color='red', arrow_length_ratio=0.1)
        
        # 4. Labeling (Fixed)
        ax.set_title(f'Qubit {i} Evolution')
        ax.set_xlabel('X', labelpad=-5)
        ax.set_ylabel('Y', labelpad=-5)
        ax.set_zlabel('Z', labelpad=-5)
        
        # Maintain aspect ratio and limits
        ax.set_xlim([-1.1, 1.1])
        ax.set_ylim([-1.1, 1.1])
        ax.set_zlim([-1.1, 1.1])
        ax.view_init(elev=20, azim=45)

    # Adjust layout to make room for colorbar
    fig.subplots_adjust(right=0.85)
    cbar_ax = fig.add_axes([0.9, 0.15, 0.02, 0.7])
    fig.colorbar(p, cax=cbar_ax, label='Simulation Time')
    
    plt.show()

if __name__ == "__main__":
    plot_bloch_spheres("simulation_7_ds.csv")