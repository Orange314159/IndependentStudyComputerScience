#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cmath>

// Use C++ complex double
using complex_t = std::complex<double>;

struct Qubit { // i will ignore having a global phase because I am ok using complex values for both alpha and beta
    complex_t alpha; // Component for |0>
    complex_t beta;  // Component for |1>
};

struct SimulationParams {
    double omega;   // 
    double delta;   // any value over 0 will show detuning
    double dt;      // Time step
};

const complex_t I(0, 1);

Qubit compute_derivative(Qubit q, SimulationParams p) {
    Qubit dq; // differential state
    // H = [[Delta, Omega], [Omega, -Delta]] * 0.5
    dq.alpha = -I * 0.5 * (p.delta * q.alpha + p.omega * q.beta);
    dq.beta  = -I * 0.5 * (p.omega * q.alpha - p.delta * q.beta);
    return dq;
}

Qubit rk4_step(Qubit q, SimulationParams p) { // to solve the schrodingers equation bc im not good enough at calculus to do it analytically
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

void get_bloch_coordinates(Qubit q, double &x, double &y, double &z) { // common function found online
    x = 2.0 * std::real(q.alpha * std::conj(q.beta));
    y = 2.0 * std::imag(std::conj(q.alpha) * q.beta);
    z = std::norm(q.alpha) - std::norm(q.beta);
}

int main() {
    

    for (int i = 0; i < 100; i ++) { // i want to run 100 simulations with different detuning values to show how the trajectory changes
        Qubit myQubit = {complex_t(1, 0), complex_t(0, 0)}; // Initial state |0>
        SimulationParams params = {1.0, 0.01*i, 0.001};

        std::ofstream file("trajectory_" + std::to_string(i) + ".csv"); // output will be saved here so I can refrence in python (manim)
        file << "time,x,y,z\n";

        for (double t = 0; t < 20.0; t += params.dt) { // simulate for 20 seconds should be long enough
            // save cords to file
            double x, y, z;
            get_bloch_coordinates(myQubit, x, y, z); // x, y, z are the bloch sphere coordinates for the current state of the qubit
            file << t << "," << x << "," << y << "," << z << "\n";
            
            // update cords
            myQubit = rk4_step(myQubit, params);
        }

        file.close();
        std::cout << "Simulation complete. Data saved to trajectory.csv" << "\n";

    }

    
    return 0;
}