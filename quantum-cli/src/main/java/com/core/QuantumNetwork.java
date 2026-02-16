package com.core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

/**
 * Represents a quantum network that enables communication between quantum computers.
 * Handles both classical and quantum message passing.
 */
public class QuantumNetwork {
    private final Map<String, QuantumComputer> quantumComputers;
    private final Queue<Message> messageQueue;
    private final Map<String, List<String>> classicalMessages;

    /**
     * Message class for network communication. 
     * Note: When java starts to implement structs I will use that then, but for now I think this is ok.
     */
    public static class Message {
        public String    from;
        public String    to;
        public Complex[] quantumData;
        public String    classicalData;
        public long      timeStamp;
        

        public Message(String from, String to, Complex[] quantumData, String classicalData) {
            this.from = from;
            this.to = to;
            this.quantumData = quantumData;
            this.classicalData = classicalData;
            this.timeStamp = System.currentTimeMillis();
        }

        @Override
        public String toString() {
            return String.format("[Message from %s to %s at %d Classical: %s", from, to, timeStamp, classicalData);
        }
    }

    /**
     * Initialize a quantum network
     */
    public QuantumNetwork() {
        this.quantumComputers = new HashMap<>();
        this.messageQueue = new LinkedList<>();
        this.classicalMessages = new HashMap<>();
    }

    /**
     * Register a quantum computer on this network.
     * @param computer The quantum computer to register to the network
     */
    public void registerQuantumComputer(QuantumComputer quantumComputer) {
        this.quantumComputers.put(quantumComputer.getName(), quantumComputer);
        this.classicalMessages.put(quantumComputer.getName(), new ArrayList<>());
        System.out.println("Register quantum computer: " + quantumComputer.getClass());
    }

    /**
     * Unregister a quantum computer from the network
     */
    public void unregisterComputer(String computerName) { 
        this.quantumComputers.remove(computerName);
        this.classicalMessages.remove(computerName);
    }

    /**
     * Send classical message over the network
     * @param fromName The sender's name
     * @param toName The receiver's name 
     * @param message The message context
     */
    public void sendClassicalMessage(String fromName, String toName, String message) { 
        if (!quantumComputers.containsKey(fromName)) {
            throw new IllegalArgumentException("Sender not found: " + fromName);
        }
        if (!quantumComputers.containsKey(toName)) {
            throw new IllegalArgumentException("Receiver not found: " + toName);
        }
        Message msg = new Message(fromName, toName, null, message);
        messageQueue.add(msg);
        classicalMessages.get(toName).add(message);
        System.out.println("Classical message sent from " + fromName + " to " + toName + ": " + message);
    }

    /**
     * Send a quantum state over the network
     * @param fromName The sender's name
     * @param toName The receiver's name
     * @param quantumState THe quantum state to send
     */
    public void sendQuantumState(String fromName, String toName, Complex[] quantumState) {
        if (!quantumComputers.containsKey(fromName)) {
            throw new IllegalArgumentException("Sender not found: " + fromName);
        }
        if (!quantumComputers.containsKey(toName)) {
            throw new IllegalArgumentException("Receiver not found: " + toName);
        }

        Message msg = new Message(fromName, toName, quantumState, null);
        messageQueue.add(msg);
        System.out.println("Quantum message sent from " + fromName + " to " + toName);
    }

    /**
     * Send entangled qubits to another computer.
     * The entangled qubits act as a shared state.
     * @param fromName The sender's name
     * @param toName The receiver's name
     * @param classicalBits The classical bits describing the shared state
     */
    public void sendEntangledState(String fromName, String toName, String classicalBits) {
        sendClassicalMessage(fromName, toName, "ENTANGLED:" + classicalBits);
    }

    /**
     * Receive a message from the queue.
     * @return The next message, or null if the queue is empty
     */
    public Message receive() {
        return messageQueue.poll();
    }

    /**
     * Get all classical messages received by a specific computer.
     * @param computerName The name of the specific computer to read the classical messages from
     * @return The list of classical messages
     */
    public List<String> getClassicalMessages(String computerName){
        return classicalMessages.getOrDefault(computerName, new ArrayList<>());
    }

    /**
     * Clear all classical messages for a specific computer.
     * @param computerName The name of the computer
     */
    public void clearMessages(String computerName) {
        if (classicalMessages.containsKey(computerName)){
            classicalMessages.put(computerName, new ArrayList<>());
        }
    }

    /**
     * Get a registered quantum computer by name.
     * @param computerName The name of the computer to get
     * @return The quantum computer, or null if not found
     */
    public QuantumComputer getComputer(String computerName) {
        return quantumComputers.get(computerName);
    }

    /**
     * Get all registered computer.
     * @return List of all quantum computers
     */
    public List<QuantumComputer> getAllQuantumComputers() {
        return new ArrayList<>(quantumComputers.values());
    }

    /**
     * Check if a computer is registered on this network
     * @param computerName The name to check
     * @return True if the computer is registered else False
     */
    public boolean hasQuantumComputer(String computerName) {
        return this.quantumComputers.containsKey(computerName);
    }

    /**
     * Get the current message queue size.
     * @return Number of pending messages
     */
    public int getMessageQueueSize() {
        return messageQueue.size();
    }

    @Override
    public String toString() {
        return "QuantumNetwork{" +
                "computers=" + quantumComputers.size() +
                ", pendingMessages=" + messageQueue.size() +
                '}';
    }

}
