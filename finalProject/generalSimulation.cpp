#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <cmath>
#include <string>
#include <random>
#include <algorithm>
#include <functional>

using complex_t = std::complex<double>;
const complex_t I(0, 1);

// qubit does not really work anymore because i will use quantum state due to krokner scaling and quantum space complexity

// struct Qubit { // no global phase so alpha and beta will both be complex
//     complex_t alpha;
//     complex_t beta; 
// };

struct QuantumState {
    std::vector<complex_t> amplitudes;
    int num_qubits;

    // Normal constructor - initializes to |000...>
    QuantumState(int n) : num_qubits(n) {
        amplitudes.resize(1 << n, 0.0);
        amplitudes[0] = 1.0;
    }

    // Zero constructor for derivatives - all amplitudes = 0
    static QuantumState zero(int n) {
        QuantumState s(n);
        s.amplitudes[0] = 0.0;  // override the 1.0
        return s;
    }
};

struct SimulationParams {
    double omega;   // Rabi Frequency
    double delta;   // Detuning
    double phi;     // Microwave Phase (determines rotation axis)
    double dt;      // Time step
};



// allow for some noise in my detuning to make it mrore realistic 
// in the future i may include my own noise generator but I do not know enough yet to think i can
//  make my own that will be better or faster in any way

// randomness used for omega
// this will contain some memory from before because long story i don't want to explain
struct SimulationNoise{
    std::default_random_engine generator;

    std::normal_distribution<double> omega_dist;
    std::normal_distribution<double> delta_dist;

    double current_omega_noise = 0.00;
    double current_delta_noise = 0.00;

    double omega_correlation = 0.95;
    double delta_correlation = 0.99; // delta is much more correlated 

    SimulationNoise() : omega_dist(0.00, 0.01), delta_dist(0.00, 0.01) { // adjust stdev and mean if needed HERE
        std::random_device rd;
        generator.seed(rd());
    }

    void updateParams(SimulationParams &p, double base_omega, double base_delta) {
        current_omega_noise = (current_omega_noise * omega_correlation) + omega_dist(generator);
        p.omega = base_omega + current_omega_noise;

        current_delta_noise = (current_delta_noise * delta_correlation) + delta_dist(generator);
        p.delta = base_delta + current_delta_noise;
    }

};


double get_last_time(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return 0.0;

    std::string line, lastLine;
    while (std::getline(file, line)) {
        if (!line.empty()) lastLine = line;
    }
    file.close();

    if (lastLine.empty() || lastLine.find("time") != std::string::npos) {
        return 0.0;
    }

    try {
        // The time is the first value in the CSV line
        size_t pos = lastLine.find(',');
        if (pos != std::string::npos) {
            return std::stod(lastLine.substr(0, pos));
        }
    } catch (...) {
        return 0.0;
    }
    return 0.0;
}

// old single qubit derivative

// Qubit compute_derivative(Qubit q, SimulationParams p) {
//     Qubit dq;

//     // Modified Hamiltonian with Phase (phi)
//     // H = 0.5 * [[Delta, Omega*e^(-i*phi)], [Omega*e^(i*phi), -Delta]]
//     complex_t drive = p.omega * std::exp(I * p.phi);
//     complex_t drive_conj = p.omega * std::exp(-I * p.phi);

//     dq.alpha = -I * 0.5 * (p.delta * q.alpha + drive_conj * q.beta);
//     dq.beta  = -I * 0.5 * (drive * q.alpha - p.delta * q.beta);
//     return dq;
// }

// new and improved state derivative 
QuantumState compute_derivative_single(const QuantumState &s, int k, double omega, double delta, double phi) {
    QuantumState ds = QuantumState::zero(s.num_qubits);

    complex_t drive      = omega * std::exp(I * phi);
    complex_t drive_conj = std::conj(drive);

    int mask = 1 << k;

    for (int i=0; i < (1<<s.num_qubits); ++i) {
        if (!(i & mask)) {
            int j = i | mask;

            ds.amplitudes[i] += -I * 0.5 * (delta * s.amplitudes[i] + drive_conj * s.amplitudes[j]);
            ds.amplitudes[j] += -I * 0.5 * (drive * s.amplitudes[i] - delta * s.amplitudes[j]);
        }
    }
    return ds;
}

QuantumState compute_derivative_cx(const QuantumState& s, int c, int t, double omega_cr) {
    QuantumState ds = QuantumState::zero(s.num_qubits);
    int control_mask = 1 << c;
    int target_mask = 1 << t;
    complex_t factor = -I * 0.5 * omega_cr;

    for (int i = 0; i < (1 << s.num_qubits); ++i) {
        int pair_idx = i ^ target_mask;
        if (i < pair_idx) { // Ensure we process each pair once
            double z_sign = (i & control_mask) ? -1.0 : 1.0;
            
            ds.amplitudes[i] += factor * z_sign * s.amplitudes[pair_idx];
            ds.amplitudes[pair_idx] += factor * z_sign * s.amplitudes[i];
        }
    }
    return ds;
}

// algorithm to approximate the derivative found online
// Qubit rk4_step(Qubit q, SimulationParams p) {
//     Qubit k1 = compute_derivative(q, p);
//     Qubit q2 = {q.alpha + k1.alpha * (p.dt/2.0), q.beta + k1.beta * (p.dt/2.0)};
//     Qubit k2 = compute_derivative(q2, p);
//     Qubit q3 = {q.alpha + k2.alpha * (p.dt/2.0), q.beta + k2.beta * (p.dt/2.0)};
//     Qubit k3 = compute_derivative(q3, p);
//     Qubit q4 = {q.alpha + k3.alpha * p.dt, q.beta + k3.beta * p.dt};
//     Qubit k4 = compute_derivative(q4, p);
    
//     Qubit res;
//     res.alpha = q.alpha + (p.dt/6.0) * (k1.alpha + 2.0*k2.alpha + 2.0*k3.alpha + k4.alpha);
//     res.beta  = q.beta  + (p.dt/6.0) * (k1.beta + 2.0*k2.beta + 2.0*k3.beta + k4.beta);
//     return res;
// }

QuantumState add_scaled(const QuantumState &base, const QuantumState &derivative, double scale) {
    QuantumState result = base;

    for (size_t i=0; i < base.amplitudes.size(); i++) {
        result.amplitudes[i] += derivative.amplitudes[i] * scale;
    }

    return result;
}

// more general rk4 stepper using functional to define a function
QuantumState rk4_step(const QuantumState &q, double dt, std::function<QuantumState(const QuantumState&)> derivative_func) {
    QuantumState k1 = derivative_func(q);
    
    QuantumState q2 = add_scaled(q, k1, dt / 2.0);
    QuantumState k2 = derivative_func(q2);
    
    QuantumState q3 = add_scaled(q, k2, dt / 2.0);
    QuantumState k3 = derivative_func(q3);
    
    QuantumState q4 = add_scaled(q, k3, dt);
    QuantumState k4 = derivative_func(q4);

    QuantumState res = q;
    double weight = dt / 6.0;
    for (size_t i = 0; i < q.amplitudes.size(); ++i) {
        res.amplitudes[i] += weight * (k1.amplitudes[i] + 2.0 * k2.amplitudes[i] + 2.0 * k3.amplitudes[i] + k4.amplitudes[i]);
    }

    double norm = 0.0;
    for (const auto& amp : res.amplitudes) norm += std::norm(amp);
    norm = std::sqrt(norm);
    for (auto& amp : res.amplitudes) amp /= norm;

    return res;
}


// given the alpha beta values you can calculate the x,y,z of the bloch sphere representation
// void get_bloch_coordinates(Qubit q, double &x, double &y, double &z) {
//     x = 2.0 * std::real(q.alpha * std::conj(q.beta));
//     y = 2.0 * std::imag(std::conj(q.alpha) * q.beta);
//     z = std::norm(q.alpha) - std::norm(q.beta);
// }

// now to get the bloch sphere projection for each qubit using k index
void get_bloch_coordinates(const QuantumState &q, int k, double &x, double &y, double &z) {
    x=0.0; y=0.0; z=0.0;

    int mask = 1 << k;

    for (int i = 0; i < (1<<q.num_qubits); ++i) {
        if (!(i & mask)) {
            int j = i | mask;

            complex_t c0 = q.amplitudes[i];
            complex_t c1 = q.amplitudes[j];

            x += 2.0 * std::real(std::conj(c0) * c1);
            
            y += 2.0 * std::imag(std::conj(c0) * c1);
            
            z += std::norm(c0) - std::norm(c1);
        }
    }
}

void simulate_added_gate(QuantumState &quantumState, std::vector<int> qubitNumbers, std::string gateName, 
                         double targetAngle, double phase, double grain, std::string filename) {
    // applies a gate to a specific qubit on a quantum computer 
    // input of a vector of numbers in the case that the gate is applied to multiple qubits at the same time
    //
    // if it is a single qubit gate then it will apply the same gate to all the listed qubits
    // if it is a multi qubit gate then it considers the first qubit the control and 
    //  the others the target qubits
    //

    // all information will be exported to a single csv file 
    // the file will follow some sort of format where every line it will print each qubits' information
    // this information will then be finally ended by the time 

    double base_omega = 1.0;
    // Duration = Angle / Omega (for a perfect pulse)
    double duration = (targetAngle / base_omega);

    double base_delta = 0.0; 
    SimulationParams params = {base_omega, base_delta, phase, grain};

    // more realistic noise model
    SimulationNoise noise;
    // noise.updateParams(params, base_omega, base_delta);

    // std::ofstream file(filename);
    // std::ifstream checkFile(filename);
    // bool isNewFile = !checkFile.is_open() || checkFile.peek() == std::ifstream::traits_type::eof();
    // checkFile.close();

    // // Open in append mode
    // std::ofstream file(filename, std::ios::app);

    // // Only write the header if this is the very first gate being logged
    // if (isNewFile) {
    //     file << "time,";
    //     for (int i = 0; i < quantumState.num_qubits; i++) {
    //         file << "q" << i << "_x,q" << i << "_y,q" << i << "_z";
    //         if (i != quantumState.num_qubits - 1) file << ",";
    //     }
    //     file << "\n";
    // }

    // static double totalTime = 0.0;
    double startTime = get_last_time(filename);
    bool isNewFile = (startTime == 0.0);
    std::ofstream file(filename, std::ios::app);

    if (isNewFile) { // you only need a header if the file is new
        file << "time,";
        for (int i = 0; i < quantumState.num_qubits; i++) {
            file << "q" << i << "_x,q" << i << "_y,q" << i << "_z";
            if (i != quantumState.num_qubits - 1) file << ",";
        }
        file << "\n";
    }

    // // setup csv header
    // file << "time,";
    // for (int i = 0; i < quantumState.num_qubits; i++) {
    //     file << "q" << i << "_x,q" << i << "_y,q" << i << "_z";
    //     if (i != quantumState.num_qubits - 1) file << ",";
    // }
    // file << "\n";

    for (double t = 0; t <= duration; t += params.dt) {
        double currentTime = startTime + t;
        file << currentTime << ",";

        // update noise params for this specific time step (it should be consistent between all qubits because physics i think?)
        noise.updateParams(params, base_omega, base_delta);

        auto total_derivative = [&](const QuantumState &s) { // capture all local variables here for my function
            QuantumState ds = QuantumState::zero(s.num_qubits);

            if (gateName == "H" || gateName == "X" || gateName == "Y" || gateName == "Z") {
                // single qubit gates
                for (int target_idx : qubitNumbers) {
                    QuantumState step_ds = compute_derivative_single(s, target_idx, params.omega, params.delta, params.phi);
                    for (size_t j = 0; j < s.amplitudes.size(); j++) {
                        ds.amplitudes[j] += step_ds.amplitudes[j];
                    }
                }
            } else if (gateName == "CX" || gateName == "CNOT") {
                if (qubitNumbers.size() >= 2) {
                    int control = qubitNumbers[0];

                    for (size_t idx = 1; idx < qubitNumbers.size(); idx++) {
                        QuantumState step_ds = compute_derivative_cx(s, control, qubitNumbers[idx], params.omega);
                        for (size_t j = 0; j<s.amplitudes.size(); j++) {
                            ds.amplitudes[j] += step_ds.amplitudes[j];
                        }
                    }
                }   
            }
            return ds;
        };

        quantumState = rk4_step(quantumState, params.dt, total_derivative);

        for (int i=0; i < quantumState.num_qubits; i++) {
            double x,y,z;

            get_bloch_coordinates(quantumState, i, x,y,z);
            file << x << "," << y << "," << z;
            if (i != quantumState.num_qubits - 1) file << ",";
        }
        file << "\n";

    }
    
    file.close();


    std::cout << "QSM: simulated " << gateName << "..." << std::endl;
}

int main() {
    const double PI = 3.141592653589793;

    QuantumState qs(10);

    std::string filename = "simulation_7.csv";
    double res = 0.01;

    /* * Target Angle and Phase for simulate_added_gate:
    * * Gate | Angle (rad) | Phase (phi) | Axis/Notes
    * ----------------------------------------------------------------
    * X    | M_PI        | 0.0         | X-axis rotation
    * Y    | M_PI        | M_PI / 2.0  | Y-axis rotation
    * Z    | M_PI        | --          | Usually handled via detuning (delta)
    * H    | M_PI        | --          | Combined X and Y/Z rotations
    * S    | M_PI / 2.0  | --          | Z-axis (detuning pulse)
    * T    | M_PI / 4.0  | --          | Z-axis (detuning pulse)
    */

    // simulate_added_gate(qs, {0,1,2,3,4}, "X", PI, 0, res, filename);

    // simulate_added_gate(qs, {1,3,4}, "Z", PI, 0, res, filename);

    // simulate_added_gate(qs, {0,1,2}, "CX", PI, 0, res, filename);
    // simulate_added_gate(qs, {3,1,4}, "CX", PI, 0, res, filename);

    // simulate_added_gate(qs, {0,2,4}, "Y", PI, PI/2, res, filename);


    // simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
    // simulate_added_gate(qs, {0}, "H", PI/2, PI/2, res, filename);
    // simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);

    // simulate_added_gate(qs, {0,1}, "CX", PI, 0, res, filename);

    // simulate_added_gate(qs, {0,1}, "Y", PI, 0, res, filename);
    // simulate_added_gate(qs, {1,2}, "X", PI, 0, res, filename);
    // simulate_added_gate(qs, {0,2}, "H", PI, 0, res, filename);

    simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {2}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {4}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {6}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {8}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {9,0,1,2,3,4,5,6,7,8}, "MCX", PI, 0, res, filename);
simulate_added_gate(qs, {9}, "H", PI/2, PI/2, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "X", PI, 0, res, filename);
simulate_added_gate(qs, {0,1,2,3,4,5,6,7,8,9}, "H", PI/2, PI/2, res, filename);


    
    return 0;
}