package com.cli;

import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

import com.core.QuantumComputer;
import com.core.QuantumNetwork;

/**
 * Command Line Interface for the quantum computer simulator.
 * Provides interactive commands for:
 *  creating quantum computers,
 *  building circuits,
 *  and executing protocols.
 */
public class QuantumCLI {
    private final QuantumNetwork network;
    private final Map<String, QuantumComputer> quantumComputers;
    private final Scanner scanner;
    private static final String PROMPT = "qit> ";

    /**
     * Init the CLI
     */
    public QuantumCLI(){
        this.network = new QuantumNetwork();
        this.quantumComputers = new HashMap<>();
        this.scanner = new Scanner(System.in);
    }

    /**
     * Main entry point.
     */
    public static void main(String[] args) {
        QuantumCLI cli = new QuantumCLI();
        if (args.length > 0 && args[0].equals("--demo")) {
            // cli.runDemo();
        } else {
            cli.start();
        }
    }

    public void start(){
        printWelcome();

        boolean running = true;

        while (running) {
            System.out.println(PROMPT);
            String input = scanner.next().trim();

            if (input.isEmpty()){
                continue;
            }

            String[] tokens  = input.split("\\s+");
            String   command = tokens[0].toLowerCase();

            try {
                switch (command) {
                    case "help", "h" -> printHelp();
                    case "create" -> handleCreate(tokens);
                    case "list" -> handleList();
                    case "show" -> handleShow(tokens);
                    case "hadamard" -> handleHadamard(tokens);
                    case "x" -> handlePauliX(tokens);
                    case "z" -> handlePauliZ(tokens);
                    case "cnot" -> handleCNOT(tokens);
                    case "measure" -> handleMeasure(tokens);
                    case "reset" -> handleReset(tokens);
                    // case "teleport" -> handleTeleport(tokens);
                    // case "entangle" -> handleEntangle(tokens);
                    // case "demo" -> runDemoMode();
                    case "quit", "q", "exit" -> {
                        running = false;
                        System.out.println("Goodbye!");
                    }
                    default -> {
                        System.out.println("Unknown command: " + command);
                        System.out.println("Type 'help' for available commands.");
                    }
                }
                
            } catch (Exception e) {
                System.out.println("Error: " + e.getMessage());
            }

        }
    }

    /**
     * Handles 'create' command.
     * Generally allow the user to create a new computer that is automatically added to the network.
     * @param tokens An array of strings that should include {create, <name>, <num_qubits>}
     */
    private void handleCreate(String[] tokens) {
        if (tokens.length < 3) {
            System.out.println("Usage: create <name> <num_qubits>");
            return;
        }

        String name = tokens[1];
        int numQubits = Integer.parseInt(tokens[2]);

        if (quantumComputers.containsKey(name)) {
            System.out.println("Computer '" + name + "' already exists.");
            return;
        }

        QuantumComputer quantumComputer = new QuantumComputer(name, numQubits);
        quantumComputers.put(name, quantumComputer);
        network.registerQuantumComputer(quantumComputer);
        System.out.println("created quantum computer '" + name + "' with " + numQubits + " qubits.");
    }

    /**
     * Handle 'list' command.
     * Generally allows the user to see a list of all quantum computers via a print to the console.
     * Will also mention if there are no quantum computers created yet.
     */
    private void handleList() {
        if (quantumComputers.isEmpty()) {
            System.out.println("No quantum computers created yet.");
            return;
        }

        System.out.println("Quantum Computers:");
        for (QuantumComputer quantumComputer : quantumComputers.values()) {
            System.out.println(" -" + quantumComputer.getName() + " (" + quantumComputer.getNumQubits() + " qubits)");
        }
    }

    /**
     * Handle 'show' command.
     * Generally allows the user to see all of the qubit states of the quantum computer.
     * @param tokens An array of strings that should include {show, <name>}
     */
    private void handleShow(String[] tokens) {
        if (tokens.length < 2) {
            System.out.println("Usage: show <computer_name>");
            return;
        }

        String name = tokens[1];
        QuantumComputer quantumComputer = quantumComputers.get(name);

        if (quantumComputer == null) {
            System.out.println("Computer was not found: " + name);
            return;
        }

        System.out.println(quantumComputer.getStateString());
    }

    /**
     * handles 'hadamard' command
     * Generally allows the user to apply the hadamard gate to a specific qubit on a specific quantum computer
     * @param tokens An array of strings that should include {hadamard, <name>, <qubit_num>}
     */
    private void handleHadamard(String[] tokens) {
        if (tokens.length < 3) {
            System.out.println("Usage: hadamard <computer_name> <qubit_index>");
            return;
        }

        QuantumComputer quantumComputer = quantumComputers.get(tokens[1]);
        int qubitIndex = Integer.parseInt(tokens[2]);

        if (quantumComputer == null) {
            System.out.println("Computer was not found");
            return;
        }

        quantumComputer.applyHadamard(qubitIndex);
        System.out.println("Applied Hadamard gate to " + tokens[1] + " qubit " + qubitIndex);
    }

    /**
     * handles 'x' command
     * Generally allows the user to apply the pauli-x gate to a specific qubit on a specific quantum computer
     * @param tokens An array of strings that should include {x, <name>, <qubit_num>}
     */
    private void handlePauliX(String[] tokens) {
        if (tokens.length < 3) {
            System.out.println("Usage: x <computer_name> <qubit_index>");
            return;
        }
        
        QuantumComputer quantumComputer = quantumComputers.get(tokens[1]);
        int qubitIndex = Integer.parseInt(tokens[2]);
        
        if (quantumComputer == null) {
            System.out.println("Computer not found");
            return;
        }
        
        quantumComputer.applyPauliX(qubitIndex);
        System.out.println("Applied Pauli-X gate to " + tokens[1] + " qubit " + qubitIndex);
    }

    /**
     * handles 'z' command
     * Generally allows the user to apply the pauli-z gate to a specific qubit on a specific quantum computer
     * @param tokens An array of strings that should include {z, <name>, <qubit_num>}
     */
    private void handlePauliZ(String[] tokens) {
        if (tokens.length < 3) {
            System.out.println("Usage: x <computer_name> <qubit_index>");
            return;
        }
        
        QuantumComputer quantumComputer = quantumComputers.get(tokens[1]);
        int qubitIndex = Integer.parseInt(tokens[2]);
        
        if (quantumComputer == null) {
            System.out.println("Computer not found");
            return;
        }
        
        quantumComputer.applyPauliZ(qubitIndex);
        System.out.println("Applied Pauli-Z gate to " + tokens[1] + " qubit " + qubitIndex);
    }

    /**
     * handles 'cnot' command
     * Generally allows the user to apply the cnot gate to a target and control gate on a specific computer
     * @param tokens An array of strings that should include {cnot, <name>, <control>, <target>}
     */
    private void handleCNOT(String[] tokens) {
        if (tokens.length < 4) {
            System.out.println("Usage: cnot <computer_name> <control> <target>");
            return;
        }
        
        QuantumComputer quantumComputer = quantumComputers.get(tokens[1]);
        int control = Integer.parseInt(tokens[2]);
        int target = Integer.parseInt(tokens[3]);
        
        if (quantumComputer == null) {
            System.out.println("Computer not found");
            return;
        }
        
        quantumComputer.applyCNOT(control, target);
        System.out.println("Applied CNOT gate to " + tokens[1] + " (control: " + control + ", target: " + target + ")");
    }

    /**
     * handles measure command
     * Generally allows the user to measure a specific qubit on a specific quantum computer
     * @param tokens An array of strings that should include {measure, <name>, <qubit>}
     */
    private void handleMeasure(String[] tokens) {
        if (tokens.length < 3) {
            System.out.println("Usage: measure <computer_name> <qubit_index>");
            return;
        }
        
        QuantumComputer quantumComputer = quantumComputers.get(tokens[1]);
        int qubitIndex = Integer.parseInt(tokens[2]);
        
        if (quantumComputer == null) {
            System.out.println("Computer not found");
            return;
        }
        
        int result = quantumComputer.measure(qubitIndex);
        System.out.println("Measured qubit " + qubitIndex + " of " + tokens[1] + ": " + result);
    }

    /**
     * handles 'reset' command
     * Generally allows the user to reset the qubits on a specific quantum computer
     * @param tokens An array of strings that should include {reset, <computer_name>}
     */
    private void handleReset(String[] tokens) {
        if (tokens.length < 2) {
            System.out.println("Usage: reset <computer_name>");
            return;
        }
        
        QuantumComputer quantumComputer = quantumComputers.get(tokens[1]);
        
        if (quantumComputer == null) {
            System.out.println("Computer not found");
            return;
        }
        
        quantumComputer.reset();
        System.out.println("Reset quantum computer '" + tokens[1] + "' to |0...0> state");
    }






    /**
     * Print welcome message
     */
    private void printWelcome() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("  QUANTUM COMPUTER SIMULATOR CLI");
        System.out.println("=".repeat(50));
        System.out.println("Simulates quantum computers and protocols");
        System.out.println("Type 'help' for available commands");
        System.out.println("Type 'demo' to run a demonstration");
        System.out.println("Type 'quit' to exit\n");
    }

    /**
     * Print help message.
     */
    private void printHelp() {
        System.out.println("\n" + "-".repeat(50));
        System.out.println("AVAILABLE COMMANDS:");
        System.out.println("-".repeat(50));
        System.out.println("create <name> <qubits>       Create a quantum computer");
        System.out.println("list                         List all quantum computers");
        System.out.println("show <name>                  Show quantum state");
        System.out.println("hadamard <name> <qubit>      Apply Hadamard gate");
        System.out.println("cnot <name> <ctl> <tgt>      Apply CNOT gate");
        System.out.println("x <name> <qubit>             Apply Pauli-X gate");
        System.out.println("z <name> <qubit>             Apply Pauli-Z gate");
        System.out.println("measure <name> <qubit>       Measure a qubit");
        System.out.println("reset <name>                 Reset to |0...0> state");
        System.out.println("entangle <name> <q1> <q2>    Create Bell pair");
        System.out.println("teleport <sender> <receiver> Execute teleportation");
        System.out.println("demo                         Run interactive demo");
        System.out.println("help                         Show this help");
        System.out.println("(q)uit                       Exit program");
        System.out.println("-".repeat(50) + "\n");
    }

}
