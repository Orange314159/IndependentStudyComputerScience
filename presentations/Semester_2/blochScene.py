from manim import *
import numpy as np


# ===============================================================
# Scene 1 - How thermal noise affects a qubit on the Bloch sphere
# ===============================================================
class HeatOnQubit(ThreeDScene):
    def construct(self):
        # Camera 
        self.set_camera_orientation(phi=70 * DEGREES, theta=30 * DEGREES)

        sphere = Sphere(radius=2, fill_opacity=0.08, color=BLUE_D,
                        resolution=(24, 24))
        equator = Circle(radius=2, color=BLUE_E).set_opacity(0.3)
        meridian = Circle(radius=2, color=BLUE_E).set_opacity(0.3)
        meridian.rotate(PI / 2, axis=RIGHT)

        axes = ThreeDAxes(
            x_range=[-2.5, 2.5, 1], y_range=[-2.5, 2.5, 1],
            z_range=[-2.5, 2.5, 1],
            x_length=5, y_length=5, z_length=5,
        )

        lbl_x = MathTex("X", color=RED_C, font_size=30).move_to(2.8 * RIGHT)
        lbl_y = MathTex("Y", color=GREEN_C, font_size=30).move_to(2.8 * UP)
        lbl_z0 = MathTex(r"|0\rangle", font_size=32).move_to(2.6 * OUT)
        lbl_z1 = MathTex(r"|1\rangle", font_size=32).move_to(2.6 * IN)

        self.add(axes, sphere, equator, meridian)
        self.add_fixed_in_frame_mobjects(lbl_x, lbl_y, lbl_z0, lbl_z1)

        self.begin_ambient_camera_rotation(rate=0.15)
        self.wait(2)

        state_theta = ValueTracker(PI / 2)
        state_phi = ValueTracker(0)
        noise_amp = ValueTracker(0.0)
        vec_len = ValueTracker(2.0)

        def _vec_end():
            t = state_theta.get_value()
            p = state_phi.get_value()
            r = vec_len.get_value()
            a = noise_amp.get_value()
            t += np.random.uniform(-a, a)
            p += np.random.uniform(-a, a)
            return np.array([
                r * np.sin(t) * np.cos(p),
                r * np.sin(t) * np.sin(p),
                r * np.cos(t),
            ])

        state_arrow = always_redraw(
            lambda: Arrow3D(ORIGIN, _vec_end(), color=YELLOW, resolution=8)
        )
        psi_lbl = always_redraw(
            lambda: MathTex(r"|\psi\rangle", color=YELLOW, font_size=28)
            .move_to(1.25 * _vec_end())
        )

        self.play(Create(state_arrow), Write(psi_lbl), run_time=1.5)
        self.wait(1)

        title = Text("Impact of Thermal Noise", font_size=36,
                      color=RED_B, weight=BOLD).to_edge(UP)
        self.add_fixed_in_frame_mobjects(title)
        self.play(FadeIn(title, shift=DOWN * 0.3))

        self.play(noise_amp.animate.set_value(0.18), run_time=2)
        self.wait(1.5)

        self.stop_ambient_camera_rotation()
        self.move_camera(phi=60 * DEGREES, theta=120 * DEGREES, run_time=3)

        sub = Text("Decoherence  (T₁ / T₂ relaxation)", font_size=26,
                    color=ORANGE).next_to(title, DOWN, buff=0.25)
        self.add_fixed_in_frame_mobjects(sub)
        self.play(FadeIn(sub, shift=DOWN * 0.2))

        self.play(
            vec_len.animate.set_value(0.4),
            noise_amp.animate.set_value(0.45),
            run_time=4,
            rate_func=smooth,
        )

        # Final camera sweep
        self.move_camera(phi=80 * DEGREES, theta=220 * DEGREES, run_time=3)
        self.wait(1)

        mixed_lbl = Text("≈ Maximally Mixed State", font_size=24,
                         color=GREY_B).next_to(sub, DOWN, buff=0.2)
        self.add_fixed_in_frame_mobjects(mixed_lbl)
        self.play(FadeIn(mixed_lbl))
        self.wait(2)


# 
# Scene 2 - How heat degrades a quantum gate (Hadamard)
# 
class HeatOnGate(ThreeDScene):
    def construct(self):
        # ── Camera ──
        self.set_camera_orientation(phi=70 * DEGREES, theta=30 * DEGREES)

        # ── Scaffold ──
        sphere = Sphere(radius=2, fill_opacity=0.06, color=GREY,
                        resolution=(24, 24))
        equator = Circle(radius=2, color=BLUE_E).set_opacity(0.25)
        meridian = Circle(radius=2, color=BLUE_E).set_opacity(0.25)
        meridian.rotate(PI / 2, axis=RIGHT)

        axes = ThreeDAxes(
            x_range=[-2.5, 2.5, 1], y_range=[-2.5, 2.5, 1],
            z_range=[-2.5, 2.5, 1],
            x_length=5, y_length=5, z_length=5,
        )

        lbl_z0 = MathTex(r"|0\rangle", font_size=32).move_to(2.6 * OUT)
        lbl_z1 = MathTex(r"|1\rangle", font_size=32).move_to(2.6 * IN)
        lbl_plus = MathTex(r"|+\rangle", font_size=32, color=GREEN_C).move_to(2.6 * RIGHT)

        self.add(axes, sphere, equator, meridian)
        self.add_fixed_in_frame_mobjects(lbl_z0, lbl_z1, lbl_plus)

        # Title 
        title = Text("Hadamard Gate  (H)  —  Ideal vs Noisy", font_size=34,
                      weight=BOLD).to_edge(UP)
        self.add_fixed_in_frame_mobjects(title)
        self.play(FadeIn(title, shift=DOWN * 0.3))

        # ── Slow orbit to establish 3‑D ──
        self.begin_ambient_camera_rotation(rate=0.12)
        self.wait(2)
        self.stop_ambient_camera_rotation()

        angle_tracker = ValueTracker(0.0)

        # Ideal arrow (ghost)
        ideal_arrow = always_redraw(lambda: Arrow3D(
            ORIGIN,
            [2 * np.sin(angle_tracker.get_value()),
             0,
             2 * np.cos(angle_tracker.get_value())],
            color=GREEN_C, resolution=8,
        ).set_opacity(0.35))

        # Ideal trace curve
        ideal_trace = always_redraw(lambda: ParametricFunction(
            lambda t: np.array([
                2 * np.sin(t), 0, 2 * np.cos(t)
            ]),
            t_range=[0, angle_tracker.get_value(), 0.05],
            color=GREEN_C,
        ).set_opacity(0.4))

        # Noisy arrow
        def _noisy_end():
            a = angle_tracker.get_value()
            if a < 0.01:
                return np.array([0.0, 0.0, 2.0])
            noise_angle = np.random.uniform(-0.12, 0.12)
            noise_y = np.random.uniform(-0.15, 0.15)
            na = a + noise_angle
            return np.array([
                2 * np.sin(na),
                noise_y,
                2 * np.cos(na),
            ])

        noisy_arrow = always_redraw(
            lambda: Arrow3D(ORIGIN, _noisy_end(), color=RED_C, resolution=8)
        )

        # Legend
        legend_ideal = Text("Ideal", font_size=22, color=GREEN_C)
        legend_noisy = Text("Noisy (with heat)", font_size=22, color=RED_C)
        legend = VGroup(legend_ideal, legend_noisy).arrange(DOWN, aligned_edge=LEFT, buff=0.15)
        legend.to_corner(DR, buff=0.5)
        self.add_fixed_in_frame_mobjects(legend)
        self.play(FadeIn(legend))

        self.add(ideal_arrow, ideal_trace, noisy_arrow)

        # Camera follows the action
        self.play(
            angle_tracker.animate.set_value(PI / 2),
            run_time=4,
            rate_func=linear,
        )

        # Camera swing to inspect the error
        self.move_camera(phi=60 * DEGREES, theta=130 * DEGREES, run_time=2.5)

        err_lbl = Text("Gate Fidelity Error", font_size=26, color=RED_B,
                        weight=BOLD).to_edge(DOWN)
        self.add_fixed_in_frame_mobjects(err_lbl)
        self.play(FadeIn(err_lbl, shift=UP * 0.3))

        # Hold the noisy jitter at the final position for a couple seconds
        self.wait(2)

        # Another camera angle
        self.move_camera(phi=85 * DEGREES, theta=250 * DEGREES, run_time=3)
        self.wait(2)