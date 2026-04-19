package com.cli;

import org.junit.Test;
import static org.junit.Assert.*;

public class QuantumCLITest {

    @Test
    public void testConstructor() {
        QuantumCLI cli = new QuantumCLI();
        assertNotNull(cli);
        // Test that network and quantumComputers are initialized
        // Since they are private, we can't directly test, but constructor should not throw
    }
}