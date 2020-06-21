import qiskit
import numpy as np
import time

constant_index_dictionary = {}
constant_index_dictionary['0000'] = [0, 2]
constant_index_dictionary['0001'] = [2, 3]
constant_index_dictionary['0010'] = [0, 1]
constant_index_dictionary['0011'] = [1, 3]
constant_index_dictionary['0100'] = [2, 3]
constant_index_dictionary['0101'] = [1, 2]
constant_index_dictionary['0110'] = [0, 2]
constant_index_dictionary['0111'] = [0, 2]
constant_index_dictionary['1000'] = [0, 3]
constant_index_dictionary['1001'] = [0, 1]
constant_index_dictionary['1010'] = [1, 2]
constant_index_dictionary['1011'] = [0, 3]
constant_index_dictionary['1100'] = [1, 3]
constant_index_dictionary['1101'] = [2, 3]
constant_index_dictionary['1110'] = [1, 3]
constant_index_dictionary['1111'] = [0, 1]



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

def controlled_n_qubit_h(qubit_list, qubit_one_list):
    output = AND_n_qubits(qubit_list, qubit_one_list)
    if(output == 1):
        circuit.h(quantum_register[AND_gate_auxillary_qubit])
        circuit.measure(quantum_register[AND_gate_auxillary_qubit], classical_register[0])
        circuit.reset(quantum_register[AND_gate_auxillary_qubit])
        job = qiskit.execute(circuit, simulator, shots=1)
        result = job.result()
        counts = str(result.get_counts())
        counts = counts[counts.find('\'') + 1:]
        counts = counts[:counts.find('\'')]
        return int(counts[len(counts) - 1])
    return 0

def recipient_initialise_starting_state(P, Q, R, S):
    if(P == "1"):
      circuit.x(quantum_register[0])
    if(Q == "1"):
      circuit.x(quantum_register[1])
    if(R == "1"):
      circuit.x(quantum_register[2])
    if(S == "1"):
      circuit.x(quantum_register[3])
    print("Message: " + str(P) + str(Q) + str(R) + str(S))

def recipient(message, signature, end_state):
  STEPS = len(signature)/len(end_state)
  STEPS = int(STEPS)
  index = 0
  recipient_initialise_starting_state(message[0], message[1], message[2], message[3])
  state = message
  recreated_signature = ""
  for _ in range(STEPS):

        dont_care_list = constant_index_dictionary[state]
        state = ""
        
        P = controlled_n_qubit_h([0, 1, 3], [1, 1, 0]) | controlled_n_qubit_h([1, 2], [0, 1]) | controlled_n_qubit_h([0, 2, 3], [0, 0, 1]) | AND_n_qubits([1, 2, 3], [0, 0, 0]) | AND_n_qubits([0, 1, 3], [1, 1, 1]) | AND_n_qubits([1, 2, 3], [1, 1, 1])
        
        Q = controlled_n_qubit_h([0, 1, 2], [1, 0, 0]) | controlled_n_qubit_h([0, 1, 2], [1, 1, 1]) | controlled_n_qubit_h([0, 2, 3], [1, 1, 1]) | controlled_n_qubit_h([1, 2, 3], [1, 1, 1]) | controlled_n_qubit_h([0, 1, 2, 3], [0, 0, 1, 0]) | controlled_n_qubit_h([0, 1, 2, 3], [0, 1, 0, 0]) | AND_n_qubits([0, 1, 3], [0, 0, 1]) | AND_n_qubits([0, 1, 2, 3], [1, 1, 0, 1]) | AND_n_qubits([0, 1, 2, 3], [1, 0, 1, 0]) | AND_n_qubits([0, 1, 2, 3], [0, 1, 1, 0])

        R = controlled_n_qubit_h([0, 1, 2], [1, 1, 0]) | controlled_n_qubit_h([0, 1, 2], [0, 0, 0]) | controlled_n_qubit_h([0, 1, 3], [0, 1, 0]) | controlled_n_qubit_h([0, 2, 3], [0, 1, 1]) | AND_n_qubits([0, 1], [1, 0]) | AND_n_qubits([0, 1, 2, 3], [0, 1, 0, 1])

        S = controlled_n_qubit_h([1, 2, 3], [1, 0, 1]) | controlled_n_qubit_h([0, 1, 3], [1, 1, 1]) | controlled_n_qubit_h([0, 1, 3], [1, 0, 0]) | controlled_n_qubit_h([0, 1, 2], [1, 0, 0]) | controlled_n_qubit_h([1, 2, 3], [0, 0, 0]) |  AND_n_qubits([0, 1, 2], [0, 0, 1]) | AND_n_qubits([0, 1, 2, 3], [0, 1, 0, 0])


        if(signature[index] != "x" and signature[index] == "1"):
          P = P | 1
        elif(signature[index] != "x"):
          P = P & 0
        
        index = index + 1

        if(signature[index] != "x" and signature[index] == "1"):
          Q = Q | 1
        elif(signature[index] != "x"):
          Q = Q & 0
        
        index = index + 1

        if(signature[index] != "x" and signature[index] == "1"):
          R = R | 1
        elif(signature[index] != "x"):
          R = R & 0

        index = index + 1

        if(signature[index] != "x" and signature[index] == "1"):
          S = S | 1
        elif(signature[index] != "x"):
          S = S & 0

        index = index + 1

        state = "" + str(P) + str(Q) + str(R) + str(S)

        hidden_state = ""
        for j in range(len(state)):
            if(j in dont_care_list):
              hidden_state = hidden_state + "x"
            else:
              hidden_state = hidden_state + state[j]
        
        print(state + " " + hidden_state)
        recreated_signature = recreated_signature + hidden_state

        for _ in range(len(circuit.data)):
          circuit.data.pop(0)

        if(P == 1):
            circuit.x(quantum_register[0])
        
        if(Q == 1):
            circuit.x(quantum_register[1])

        if(R == 1):
            circuit.x(quantum_register[2])

        if(S == 1):
            circuit.x(quantum_register[3])
  print(recreated_signature)
  print(signature)
  if(recreated_signature == signature):
    print("ACCEPT")
  else:
    print("REJECT")


start = time.time()
for _ in range(len(circuit.data)):
  circuit.data.pop(0)
recipient("1011", "x10x10xxx10x1x1xxx101xx01xx1x01x1x0xx11x0x1xx1x0x0x0xx11", "1111")
for _ in range(len(circuit.data)):
  circuit.data.pop(0)
recipient("1011", "x00x10xxx10x1x1xxx101xx01xx1x01x1x0xx11x0x1xx1x0x0x0xx11", "1111")
print(time.time() - start)