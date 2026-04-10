#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

using complex_t = std::complex<double>;
const complex_t I(0, 1);

struct State2Q {
    complex_t c00, c01, c10, c11;
};

State2Q compute_derivative_2q(State2Q s, double omega, double delta) {
    State2Q ds;

    // Control is |0>: No microwave push, but still feels 'background' delta
    ds.c00 = -I * 0.5 * (delta * s.c00); 
    ds.c01 = -I * 0.5 * (-delta * s.c01);
    
    // Control is |1>: This is where the CNOT pulse is active
    // We add 'delta' here to show what happens when the microwave 
    // frequency is "off" compared to the target qubit's frequency.
    ds.c10 = -I * 0.5 * (delta * s.c10 + omega * s.c11);
    ds.c11 = -I * 0.5 * (omega * s.c10 - delta * s.c11);
    
    return ds;
}

// RK4 for 2-qubit state
State2Q rk4_step_2q(State2Q s, double omega, double delta, double dt) {
    auto k1 = compute_derivative_2q(s, omega, delta);
    State2Q s2 = {s.c00 + k1.c00*dt/2.0, s.c01 + k1.c01*dt/2.0, s.c10 + k1.c10*dt/2.0, s.c11 + k1.c11*dt/2.0};
    auto k2 = compute_derivative_2q(s2, omega, delta);
    
    return {s.c00 + k1.c00*dt, s.c01 + k1.c01*dt, s.c10 + k1.c10*dt, s.c11 + k1.c11*dt};
}

int main() {
    State2Q state = {0.0, 0.0, 1.0, 0.0}; // Start in |10> (Control is 1, Target is 0)
    double omega = 1.0, delta = 0.5, dt = 0.01;

    std::ofstream file("cnot_data.csv");
    file << "time,p00,p01,p10,p11\n";

    for (double t = 0; t < 3.14159; t += dt) {
        file << t << "," << std::norm(state.c00) << "," << std::norm(state.c01) << "," 
             << std::norm(state.c10) << "," << std::norm(state.c11) << "\n";
        state = rk4_step_2q(state, omega, delta, dt);
    }
    return 0;
}