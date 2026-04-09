import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from pathlib import Path
from mpl_toolkits.mplot3d import Axes3D


## a bit of debug because it couldnt find the files
# print(Path('.\\finalProject').cwd()) 
# files = list(Path('.\\finalProject').glob('trajectory_*.csv'))
# print(f"Found {len(files)} files: {files}")


trajectory_files = sorted(
    Path('.\\finalProject').glob('traj_X_delta_*.csv'),
    key=lambda path: int(path.stem.split('_')[-1]) if path.stem.split('_')[-1].isdigit() else float('inf')
)

if not trajectory_files:
    print("No trajectory_*.csv files found in the current folder.")
    exit(1)

required_columns = ['x', 'y', 'z', 'time']
trajectories = []

for path in trajectory_files:
    df = pd.read_csv(path)
    df = df.rename(columns={col: col.strip().lower() for col in df.columns})
    missing_columns = [col for col in required_columns if col not in df.columns]
    if missing_columns:
        print(f"Skipping {path.name}: missing columns {missing_columns}")
        continue
    trajectories.append((path.name, df))

if not trajectories:
    print("No valid trajectory files were loaded.")
    exit(1)

fig = plt.figure(figsize=(10, 10))
ax = fig.add_subplot(111, projection='3d')

u = np.linspace(0, 2 * np.pi, 100)
v = np.linspace(0, np.pi, 100)
x_sphere = np.outer(np.cos(u), np.sin(v))
y_sphere = np.outer(np.sin(u), np.sin(v))
z_sphere = np.outer(np.ones(np.size(u)), np.cos(v))
ax.plot_wireframe(x_sphere, y_sphere, z_sphere, color="grey", alpha=0.1, linewidth=0.5)

colors = plt.cm.viridis(np.linspace(0, 1, len(trajectories)))
for color, (name, df) in zip(colors, trajectories):
    ax.plot(df['x'], df['y'], df['z'], color=color, alpha=0.7, linewidth=1)

first_name, first_df = trajectories[0]
last_name, last_df = trajectories[-1]
ax.scatter(first_df['x'].iloc[0], first_df['y'].iloc[0], first_df['z'].iloc[0], color='green', s=80, label='Start (first file)')
ax.scatter(last_df['x'].iloc[-1], last_df['y'].iloc[-1], last_df['z'].iloc[-1], color='red', s=80, label='End (last file)')

cmap = plt.cm.viridis
norm = plt.Normalize(vmin=0, vmax=len(trajectories) - 1)
cb = fig.colorbar(plt.cm.ScalarMappable(norm=norm, cmap=cmap), ax=ax, fraction=0.03, pad=0.1)
cb.set_label('Trajectory index')
cb.set_ticks([0, len(trajectories) - 1])
cb.set_ticklabels(['0', str(len(trajectories) - 1)])

ax.set_xlabel('X (Superposition)')
ax.set_ylabel('Y (Phase)')
ax.set_zlabel('Z (States |0> to |1>)')
ax.set_title(f'Microwave Pulse Simulation for a Pauli-X Gate\n({len(trajectories)} trajectories)')
ax.legend(loc='upper right')

# Set view for better perspective
ax.view_init(elev=20, azim=45)

plt.show()