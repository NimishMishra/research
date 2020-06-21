import numpy as np
import qiskit
import time

simulator = qiskit.Aer.get_backend('qasm_simulator')
circuit_000 = qiskit.QuantumCircuit(3, 3)
circuit_001 = qiskit.QuantumCircuit(3, 3)
circuit_010 = qiskit.QuantumCircuit(3, 3)
circuit_011 = qiskit.QuantumCircuit(3, 3)
circuit_100 = qiskit.QuantumCircuit(3, 3)
circuit_101 = qiskit.QuantumCircuit(3, 3)
circuit_110 = qiskit.QuantumCircuit(3, 3)
circuit_111 = qiskit.QuantumCircuit(3, 3)
def create_circuits():

    ################ the 000 circuit ###################
    circuit_000.ch(2, 0)
    circuit_000.cx(0, 1)
    circuit_000.cx(0, 2)
    circuit_000.measure(range(3), range(3))    
    
    ################# the 001 circuit #################################
    circuit_001.x(2)
    circuit_001.ch(2, 0)
    circuit_001.cx(0, 1)
    circuit_001.cx(0, 2)
    circuit_001.measure(range(3), range(3))

    ################# the 010 circuit #################################
    circuit_010.x(1)
    circuit_010.ch(2, 0)
    circuit_010.cx(0, 1)
    circuit_010.cx(0, 2)
    circuit_010.measure(range(3), range(3))

    ################# the 011 circuit #################################
    circuit_011.x(1)
    circuit_011.x(2)
    circuit_011.ch(2, 0)
    circuit_011.cx(0, 1)
    circuit_011.cx(0, 2)
    circuit_011.measure(range(3), range(3))

    ################# the 100 circuit #################################
    circuit_100.x(0)
    circuit_100.ch(2, 0)
    circuit_100.cx(0, 1)
    circuit_100.cx(0, 2)
    circuit_100.measure(range(3), range(3))

    ################# the 101 circuit #################################
    circuit_101.x(0)
    circuit_101.x(2)
    circuit_101.ch(2, 0)
    circuit_101.cx(0, 1)
    circuit_101.cx(0, 2)
    circuit_101.measure(range(3), range(3))

    ################# the 110 circuit #################################
    circuit_110.x(0)
    circuit_110.x(1)
    circuit_110.ch(2, 0)
    circuit_110.cx(0, 1)
    circuit_110.cx(0, 2)
    circuit_110.measure(range(3), range(3))

    ################# the 111 circuit #################################
    circuit_111.x(0)
    circuit_111.x(1)
    circuit_111.x(2)
    circuit_111.ch(2, 0)
    circuit_111.cx(0, 1)
    circuit_111.cx(0, 2)
    circuit_111.measure(range(3), range(3))



def execute_circuit(input):
    if(input == "000"):
        job = qiskit.execute(circuit_000, simulator, shots=1)
    elif(input == "001"):
        job = qiskit.execute(circuit_001, simulator, shots=1)
    elif(input == "010"):
        job = qiskit.execute(circuit_010, simulator, shots=1)
    elif(input == "011"):
        job = qiskit.execute(circuit_011, simulator, shots=1)
    elif(input == "100"):
        job = qiskit.execute(circuit_100, simulator, shots=1)
    elif(input == "101"):
        job = qiskit.execute(circuit_101, simulator, shots=1)
    elif(input == "110"):
        job = qiskit.execute(circuit_110, simulator, shots=1)
    elif(input == "111"):
        job = qiskit.execute(circuit_111, simulator, shots=1)


    
    result = job.result()
    counts = str(result.get_counts())
    counts = counts[counts.find('\'') + 1:]
    counts = counts[:counts.find('\'')]
    print(counts[::-1])


create_circuits()
for i in range(100):    
    execute_circuit("111")