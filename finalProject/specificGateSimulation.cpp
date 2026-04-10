#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cmath>
#include <string>
#include <random>

using complex_t = std::complex<double>;
const complex_t I(0, 1);

struct Qubit {
    complex_t alpha;
    complex_t beta;
};

struct SimulationParams {
    double omega;   // Rabi Frequency
    double delta;   // Detuning
    double phi;     // Microwave Phase (determines rotation axis)
    double dt;      // Time step
};

std::default_random_engine generator;
std::normal_distribution<double> noise_dist(0.0, 1.2); // Mean 0, StdDev 1.2


Qubit compute_derivative(Qubit q, SimulationParams p) {
    Qubit dq;
    // Modified Hamiltonian with Phase (phi)
    // H = 0.5 * [[Delta, Omega*e^(-i*phi)], [Omega*e^(i*phi), -Delta]]
    complex_t drive = p.omega * std::exp(I * p.phi);
    complex_t drive_conj = p.omega * std::exp(-I * p.phi);

    // more realistic noise model
    double stochastic_delta = p.delta + noise_dist(generator);

    dq.alpha = -I * 0.5 * (stochastic_delta * q.alpha + drive_conj * q.beta);
    dq.beta  = -I * 0.5 * (drive * q.alpha - stochastic_delta * q.beta);
    return dq;
}

Qubit rk4_step(Qubit q, SimulationParams p) {
    Qubit k1 = compute_derivative(q, p);
    Qubit q2 = {q.alpha + k1.alpha * (p.dt/2.0), q.beta + k1.beta * (p.dt/2.0)};
    Qubit k2 = compute_derivative(q2, p);
    Qubit q3 = {q.alpha + k2.alpha * (p.dt/2.0), q.beta + k2.beta * (p.dt/2.0)};
    Qubit k3 = compute_derivative(q3, p);
    Qubit q4 = {q.alpha + k3.alpha * p.dt, q.beta + k3.beta * p.dt};
    Qubit k4 = compute_derivative(q4, p);
    
    Qubit res;
    res.alpha = q.alpha + (p.dt/6.0) * (k1.alpha + 2.0*k2.alpha + 2.0*k3.alpha + k4.alpha);
    res.beta  = q.beta  + (p.dt/6.0) * (k1.beta + 2.0*k2.beta + 2.0*k3.beta + k4.beta);
    return res;
}

void get_bloch_coordinates(Qubit q, double &x, double &y, double &z) {
    x = 2.0 * std::real(q.alpha * std::conj(q.beta));
    y = 2.0 * std::imag(std::conj(q.alpha) * q.beta);
    z = std::norm(q.alpha) - std::norm(q.beta);
}

void run_gate_simulation(std::string gateName, double targetAngle, double phase, int numRuns) {
    double omega_fixed = 1.0;
    // Duration = Angle / Omega (for a perfect pulse)
    double duration = targetAngle / omega_fixed;

    for (int i = 0; i < numRuns; i++) {
        Qubit myQubit = {complex_t(1, 0), complex_t(0, 0)};
        double current_delta = 0; 
        SimulationParams params = {omega_fixed, current_delta, phase, 0.001};

        // std::string filename = "traj_" + gateName + "_delta_" + std::to_string(i) + ".csv";
        
        std::string filename = "traj_" + gateName + "_specialDELTA_.csv";
        std::ofstream file(filename);
        file << "time,x,y,z\n";

        for (double t = 0; t <= duration; t += params.dt) {
            double x, y, z;
            get_bloch_coordinates(myQubit, x, y, z);
            file << t << "," << x << "," << y << "," << z << "\n";
            myQubit = rk4_step(myQubit, params);
        }
        file.close();
    }
    std::cout << "Finished " << numRuns << " simulations for " << gateName << " gate." << std::endl;
}

int main() {
    const double PI = 3.141592653589793;

    // Simulate X Gate (PI rotation around X-axis, phase = 0)
    run_gate_simulation("X", PI, 0.0, 1);

    // Simulate H Gate (PI/2 rotation around Y-axis, phase = PI/2)
    // Note: This is a "Square root of Y" pulse, common proxy for visualizing H
    run_gate_simulation("H", PI / 2.0, PI / 2.0, 1);

    return 0;
}