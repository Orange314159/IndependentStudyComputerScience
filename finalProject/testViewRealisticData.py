import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D


def find_realistic_csv(script_dir: Path, provided: str | None = None) -> Path:
    if provided:
        candidate = Path(provided)
        if not candidate.is_absolute():
            candidate = script_dir / candidate
        if candidate.exists():
            return candidate
        raise FileNotFoundError(f"Specified file not found: {candidate}")

    candidate_files = []
    candidate_files.extend(sorted(script_dir.glob('traj_*_realData.csv')))
    candidate_files.extend(sorted(script_dir.glob('*realData*.csv')))
    candidate_files.extend(sorted(script_dir.glob('testViewRealisticData.csv')))

    if not candidate_files:
        raise FileNotFoundError(
            'No realistic simulation CSV file found in the same folder as this script. '
            'Generate the data with realisticGates.exe or pass a CSV path as an argument.'
        )

    return candidate_files[0]


def load_data(file_path: Path) -> pd.DataFrame:
    df = pd.read_csv(file_path)
    df.columns = [col.strip().lower() for col in df.columns]

    required_columns = ['time', 'x', 'y', 'z']
    missing = [col for col in required_columns if col not in df.columns]
    if missing:
        raise ValueError(
            f'CSV file {file_path.name} must contain columns: {required_columns}. Missing: {missing}'
        )

    return df


def get_microwave_column(df: pd.DataFrame) -> str | None:
    for candidate in ['microwave_val', 'mw_val', 'microwave', 'mw']:
        if candidate in df.columns:
            return candidate
    return None


def main() -> None:
    parser = argparse.ArgumentParser(
        description='Plot realistic qubit response and microwave pulse data from CSV.'
    )
    parser.add_argument(
        'csv_path',
        nargs='?',
        help='Optional path to the realistic simulation CSV file.',
    )
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    data_file = find_realistic_csv(script_dir, args.csv_path)
    df = load_data(data_file)

    microwave_col = get_microwave_column(df)
    print(f'Loaded {data_file.name} with columns: {list(df.columns)}')

    fig = plt.figure(figsize=(10, 10))
    ax1 = fig.add_subplot(2, 1, 1, projection='3d')
    ax2 = fig.add_subplot(2, 1, 2)

    u = np.linspace(0, 2 * np.pi, 100)
    v = np.linspace(0, np.pi, 100)
    x_sphere = np.outer(np.cos(u), np.sin(v))
    y_sphere = np.outer(np.sin(u), np.sin(v))
    z_sphere = np.outer(np.ones(np.size(u)), np.cos(v))
    ax1.plot_wireframe(x_sphere, y_sphere, z_sphere, color='grey', alpha=0.2, linewidth=0.5)

    ax1.plot(df['x'], df['y'], df['z'], color='blue', linewidth=2, label='Bloch trajectory')
    ax1.scatter(df['x'].iloc[0], df['y'].iloc[0], df['z'].iloc[0], color='green', s=60, label='Start')
    ax1.scatter(df['x'].iloc[-1], df['y'].iloc[-1], df['z'].iloc[-1], color='red', s=60, label='End')

    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    ax1.set_title('Bloch Sphere Trajectory')
    ax1.legend(loc='upper right')
    ax1.view_init(elev=20, azim=45)

    if microwave_col is not None:
        ax2.plot(df['time'], df[microwave_col], color='red', linewidth=1, label='Microwave Field')
        ax2.fill_between(df['time'], df[microwave_col], color='red', alpha=0.2)
        ax2.set_ylabel('Amplitude')
        ax2.legend(loc='upper right')
    else:
        ax2.text(
            0.5,
            0.5,
            'No microwave data found in this CSV file.',
            ha='center',
            va='center',
            transform=ax2.transAxes,
            fontsize=12,
            color='gray',
        )
        ax2.set_ylabel('Amplitude')

    ax2.set_xlabel('Time (ns)')
    ax2.set_title('Microwave Pulse')
    plt.tight_layout()
    plt.show()


if __name__ == '__main__':
    main()