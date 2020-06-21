import qiskit
import numpy as np
import time

CLASSICAL_REGISTER_LENGTH  = 5
QUANTUM_REGISTER_LENGTH  = 5

circuit_building_start_time = time.time()
simulator = qiskit.Aer.get_backend('qasm_simulator')
classical_register = qiskit.ClassicalRegister(CLASSICAL_REGISTER_LENGTH)
quantum_register = qiskit.QuantumRegister(QUANTUM_REGISTER_LENGTH)
circuit = qiskit.QuantumCircuit(quantum_register, classical_register)
circuit_building_end_time = time.time()


AND_gate_auxillary_qubit =  QUANTUM_REGISTER_LENGTH - 1 # last qubit as the auxillary qubit

'''
    Applies quantum AND operation to specified pair of qubits, stores the operation in AND_gate_auxillary_qubit,
    and stores the result in a classical register
    @PARAMS:
        qubit1: position of the first qubit
        qubit2: position of the second qubit
        qubit1_one: whether the first qubit is NOT
        qubit2_one: whether the second qubit is NOT
        classical_register_position: the classical register position to store the measurement of AND_gate_auxillary_qubit
'''
def AND_2_qubit(qubit1, qubit2, qubit1_one, qubit2_one, classical_register_position):
    
    if(qubit1_one):
        circuit.x(quantum_register[qubit1])
    if(qubit2_one):
        circuit.x(quantum_register[qubit2])
    circuit.ccx(quantum_register[qubit1], quantum_register[qubit2], quantum_register[AND_gate_auxillary_qubit])
    circuit.measure(quantum_register[AND_gate_auxillary_qubit], classical_register[classical_register_position])
    if(qubit1_one):
        circuit.x(quantum_register[qubit1])
    if(qubit2_one):
        circuit.x(quantum_register[qubit2])
    circuit.reset(quantum_register[AND_gate_auxillary_qubit])

'''
    Applies the AND gate operation on a list of n qubits
    @PARAMS:
        qubit_list: list of qubits to perform the operation on
        qubit_one_list: whether each of those qubits is NOT
    @RETURN:
        result of the n-qubit AND operation
'''
def AND_n_qubits(qubit_list, qubit_one_list):
    
    length = len(qubit_list)
    if(length != len(qubit_one_list)):
        print("Incorrect dimensions")
        return
    classical_register_index = 0 # where to store pairwise AND operation results

    # handling odd number of qubits by preprocessing the last qubit
    if(length % 2 != 0):
        if(qubit_one_list[length - 1] == 1):
            circuit.x(quantum_register[qubit_list[length-1]])
        circuit.cx(quantum_register[qubit_list[length - 1]], quantum_register[AND_gate_auxillary_qubit])
        circuit.measure(quantum_register[AND_gate_auxillary_qubit], classical_register[classical_register_index])
        circuit.reset(quantum_register[AND_gate_auxillary_qubit])
        classical_register_index = classical_register_index + 1
        if(qubit_one_list[length - 1] == 1):
            circuit.x(quantum_register[qubit_list[length-1]])
        length = length - 1


    for index in range(length - 1, 0, -2):
        AND_2_qubit(qubit_list[index], qubit_list[index - 1], qubit_one_list[index], qubit_one_list[index - 1], classical_register_index)
        classical_register_index = classical_register_index + 1
    
    job = qiskit.execute(circuit, simulator, shots=1)
    result = job.result()
    counts = str(result.get_counts())
    counts = counts[counts.find('\'') + 1:]
    counts = counts[:counts.find('\'')]
    output = 1
    for index in range(0, classical_register_index, 1):
        output = output & int(counts[CLASSICAL_REGISTER_LENGTH - 1 - index])
    
    return output


'''
    the main circuit for the following truth table:
    A, B, C = binary representation input state for the robot
    P, Q, R = binary representation of the output state from the robot

    A   B   C           P   Q   R

    0   0   0           0   1   0
    0   0   1           0   0   1
    0   1   0           1   0   0
    0   1   1           0   1   0
    1   0   0           1   0   0
    1   0   1           0   0   1
    1   1   0           0   1   0
    1   1   1           1   0   0

    P = qubit 0
    Q = qubit 1
    R = qubit 2
'''

def main_circuit():
    STEPS = 10 # how many steps the robot takes
    for _ in range(STEPS):
        P = AND_n_qubits([0, 1, 2], [0, 0, 0]) | AND_n_qubits([0, 1, 2], [0, 1, 1]) | AND_n_qubits([0, 1, 2], [1, 0, 1])
        Q = AND_n_qubits([0, 1, 2], [1, 1, 1]) | AND_n_qubits([0, 1, 2], [1, 0, 0]) | AND_n_qubits([0, 1, 2], [0, 0, 1])
        R = AND_n_qubits([1, 2], [1, 0])
        
        print("State: " + str(P) + str(Q) + str(R))

        if(P == 1):
            circuit.reset(quantum_register[0])
            circuit.x(quantum_register[0])
        else:
            circuit.reset(quantum_register[0])
        
        if(Q == 1):
            circuit.reset(quantum_register[1])
            circuit.x(quantum_register[1])
        else:
            circuit.reset(quantum_register[1])

        if(R == 1):
            circuit.reset(quantum_register[2])
            circuit.x(quantum_register[2])
        else:
            circuit.reset(quantum_register[2])
        

start_time = time.time()
circuit.x(quantum_register[1])
circuit.x(quantum_register[2])
print("Initial state: 011")
main_circuit()
end_time = time.time()
print("Run in time " + str(end_time - start_time))
print(circuit_building_end_time - circuit_building_start_time)