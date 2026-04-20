from manim import *
import pandas as pd
import numpy as np

class QubitPathAnimation(ThreeDScene):
    def construct(self):
        # 1. Load and Prepare Data
        try:
            raw_data = pd.read_csv('qubit_data.csv')
            
            # Change step_size to 1 to include EVERY point.
            # If 130k points is too slow to render, try 5 or 10.
            # The interpolation logic ensures it won't "jump."
            step_size = 1 
            data = raw_data.iloc[::step_size].reset_index(drop=True)
            
        except FileNotFoundError:
            print("CSV not found. Please ensure 'qubit_data.csv' is in the directory.")
            return

        # 2. Configuration
        qubits_to_track = {
            'q0': RED,
            # 'q1': BLUE,
            # 'q2': GREEN,
            # 'q3': ORANGE
        }

        # 3. Scene Setup
        axes = ThreeDAxes(
            x_range=[-1.5, 1.5, 0.5],
            y_range=[-1.5, 1.5, 0.5],
            z_range=[-1.5, 1.5, 0.5],
            x_length=6,
            y_length=6,
            z_length=6
        )

        sphere = Surface(
            lambda u, v: axes.c2p(
                np.cos(u) * np.cos(v),
                np.cos(u) * np.sin(v),
                np.sin(u)
            ),
            u_range=[-np.pi/2, np.pi/2],
            v_range=[0, TAU],
            resolution=(16, 32),
            fill_opacity=0.1,
            checkerboard_colors=[GREY_B, GREY_C],
            stroke_width=0.1
        )

        self.set_camera_orientation(phi=75 * DEGREES, theta=30 * DEGREES)
        self.add(axes, sphere)

        # 4. Animation Logic
        max_index = len(data) - 1
        tracker = ValueTracker(0)

        for q_name, color in qubits_to_track.items():
            x_col, y_col, z_col = f"{q_name}_x", f"{q_name}_y", f"{q_name}_z"

            if x_col not in data.columns:
                continue

            # Initial position
            start_pos = axes.c2p(data[x_col].iloc[0], data[y_col].iloc[0], data[z_col].iloc[0])
            dot = Dot3D(point=start_pos, color=color, radius=0.05)
            
            # TracedPath handles the "drawing" as the dot moves
            path = TracedPath(dot.get_center, stroke_color=color, stroke_width=3)
            self.add(dot, path)

            # Rigorous updater with linear interpolation
            def get_updater(qx, qy, qz):
                def updater(mobj):
                    val = tracker.get_value()
                    idx_low = int(np.floor(val))
                    idx_high = int(np.ceil(val))
                    
                    if idx_high > max_index:
                        idx_high = max_index
                    
                    alpha = val - idx_low
                    
                    # Interpolate between the current and next row for perfectly smooth motion
                    x = np.interp(alpha, [0, 1], [data[qx].iloc[idx_low], data[qx].iloc[idx_high]])
                    y = np.interp(alpha, [0, 1], [data[qy].iloc[idx_low], data[qy].iloc[idx_high]])
                    z = np.interp(alpha, [0, 1], [data[qz].iloc[idx_low], data[qz].iloc[idx_high]])
                    
                    mobj.move_to(axes.c2p(x, y, z))
                return updater

            dot.add_updater(get_updater(x_col, y_col, z_col))

        # 5. Run Animation
        self.begin_ambient_camera_rotation(rate=0.15)
        
        # INCREASE run_time for 130k points. 
        # 6 seconds would be way too fast for that much data.
        self.play(
            tracker.animate.set_value(max_index), 
            run_time=216, 
            rate_func=linear
        )

        self.stop_ambient_camera_rotation()
        self.wait(2)