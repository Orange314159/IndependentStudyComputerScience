#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

using complex_t = std::complex<double>;
const complex_t I(0, 1);

struct State2Q {
    complex_t c00, c01, c10, c11;

    State2Q operator+(const State2Q& other) const {
        return {c00 + other.c00, c01 + other.c01, c10 + other.c10, c11 + other.c11};
    }

    State2Q operator*(double scalar) const {
        return {c00 * scalar, c01 * scalar, c10 * scalar, c11 * scalar};
    }
};

State2Q compute_derivative_2q(State2Q s, double omega, double delta) {
    State2Q ds;
    // No microwave pulse on control |0>, but still feels detuning delta
    ds.c00 = -I * 0.5 * (delta * s.c00); 
    ds.c01 = -I * 0.5 * (-delta * s.c01);
    
    // Control is |1> target qubit experiences the Rabi drive omega
    ds.c10 = -I * 0.5 * (delta * s.c10 + omega * s.c11);
    ds.c11 = -I * 0.5 * (omega * s.c10 - delta * s.c11);
    
    return ds;
}

State2Q rk4_step_2q(State2Q s, double omega, double delta, double dt) {
    
    // k1 = f(t, y)
    State2Q k1 = compute_derivative_2q(s, omega, delta);
    
    // k2 = f(t + dt/2, y + dt/2 * k1)
    State2Q k2 = compute_derivative_2q(s + k1 * (dt / 2.0), omega, delta);
    
    // k3 = f(t + dt/2, y + dt/2 * k2)
    State2Q k3 = compute_derivative_2q(s + k2 * (dt / 2.0), omega, delta);
    
    // k4 = f(t + dt, y + dt * k3)
    State2Q k4 = compute_derivative_2q(s + k3 * dt, omega, delta);
    
    // Final update: s + (dt/6) * (k1 + 2k2 + 2k3 + k4)
    State2Q weighted_sum = k1 + (k2 * 2.0) + (k3 * 2.0) + k4;
    return s + weighted_sum * (dt / 6.0);
}

int main() {
    State2Q state = {0.0, 0.0, 1.0, 0.0}; // Start in |10> so we can see the effect of the CNOT on the target qubit
    double omega = 1.0, delta = 0.5, dt = 0.01;

    std::ofstream file("cnot_data.csv");
    if (!file.is_open()) return 1;

    file << "time,p00,p01,p10,p11\n";

    for (double t = 0; t < 6.28; t += dt) {
        file << t << "," << std::norm(state.c00) << "," << std::norm(state.c01) << "," 
             << std::norm(state.c10) << "," << std::norm(state.c11) << "\n";
        
        state = rk4_step_2q(state, omega, delta, dt);
    }

    std::cout << "Simulation complete. Data saved to cnot_data.csv" << std::endl;


    // now do the same thing, but with starting with |+> on the control qubit to show the entanglement effect on the CNOT
    state = {0.5, 0, 0.5, 0};
    // keep the same omega, delta, and dt 

    std::ofstream file2("cnot_data_hadamard.csv");

    if (!file2.is_open()) return 1;

    file2 << "time,p00,p01,p10,p11\n";

    for (double t = 0; t < 6.28; t += dt) {
        file2 << t << "," << std::norm(state.c00) << "," << std::norm(state.c01) << ","
              << std::norm(state.c10) << "," << std::norm(state.c11) << "\n";
        
        state = rk4_step_2q(state, omega, delta, dt);
    }

    std::cout << "Simulation complete. Data saved to cnot_data_hadamard.csv" << std::endl;

    return 0;
}