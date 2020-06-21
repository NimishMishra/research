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

'''
    the main circuit for the following truth table:
    A, B, C, D = binary representation input state for the robot
    P, Q, R, S = binary representation of the output state from the robot

    New circuit in register...
'''

def main_circuit(STEPS, initial_state):
    signature = ""
    state = initial_state
    used_states = []
    step = 0
    rollback_count = 0
    while True:
        if(step == STEPS):
          break
        dont_care_list = constant_index_dictionary[state]
        rollback_state = state
        if(state not in used_states):
          used_states.append(state)
        state = ""

        P = controlled_n_qubit_h([0, 1, 3], [1, 1, 0]) | controlled_n_qubit_h([1, 2], [0, 1]) | controlled_n_qubit_h([0, 2, 3], [0, 0, 1]) | AND_n_qubits([1, 2, 3], [0, 0, 0]) | AND_n_qubits([0, 1, 3], [1, 1, 1]) | AND_n_qubits([1, 2, 3], [1, 1, 1])
        
        Q = controlled_n_qubit_h([0, 1, 2], [1, 0, 0]) | controlled_n_qubit_h([0, 1, 2], [1, 1, 1]) | controlled_n_qubit_h([0, 2, 3], [1, 1, 1]) | controlled_n_qubit_h([1, 2, 3], [1, 1, 1]) | controlled_n_qubit_h([0, 1, 2, 3], [0, 0, 1, 0]) | controlled_n_qubit_h([0, 1, 2, 3], [0, 1, 0, 0]) | AND_n_qubits([0, 1, 3], [0, 0, 1]) | AND_n_qubits([0, 1, 2, 3], [1, 1, 0, 1]) | AND_n_qubits([0, 1, 2, 3], [1, 0, 1, 0]) | AND_n_qubits([0, 1, 2, 3], [0, 1, 1, 0])

        R = controlled_n_qubit_h([0, 1, 2], [1, 1, 0]) | controlled_n_qubit_h([0, 1, 2], [0, 0, 0]) | controlled_n_qubit_h([0, 1, 3], [0, 1, 0]) | controlled_n_qubit_h([0, 2, 3], [0, 1, 1]) | AND_n_qubits([0, 1], [1, 0]) | AND_n_qubits([0, 1, 2, 3], [0, 1, 0, 1])

        S = controlled_n_qubit_h([1, 2, 3], [1, 0, 1]) | controlled_n_qubit_h([0, 1, 3], [1, 1, 1]) | controlled_n_qubit_h([0, 1, 3], [1, 0, 0]) | controlled_n_qubit_h([0, 1, 2], [1, 0, 0]) | controlled_n_qubit_h([1, 2, 3], [0, 0, 0]) |  AND_n_qubits([0, 1, 2], [0, 0, 1]) | AND_n_qubits([0, 1, 2, 3], [0, 1, 0, 0])

        state = state + str(P) + str(Q) + str(R) + str(S)
        if(state in used_states):
          rollback_count = rollback_count + 1
          if(rollback_count == (len(initial_state) + 10)):
            print("Aborting.")
            return "ABORT"
          P = rollback_state[0]
          Q = rollback_state[1]
          R = rollback_state[2]
          S = rollback_state[3]
          state = rollback_state
          for _ in range(len(circuit.data)):
            circuit.data.pop(0)

          if(P == '1'):
            print("Rollback reset")
            circuit.x(quantum_register[0])
        
          if(Q == '1'):
            print("Rollback reset")
            circuit.x(quantum_register[1])

          if(R == '1'):
            print("Rollback reset")
            circuit.x(quantum_register[2])

          if(S == '1'):
            print("Rollback reset")
            circuit.x(quantum_register[3])
          print("Rolling back")
          continue

        step = step + 1        
        rollback = 0
        hidden_state = ""
        for j in range(len(state)):
            if(j in dont_care_list):
              hidden_state = hidden_state + "x"
            else:
              hidden_state = hidden_state + state[j]
        
        signature = signature + hidden_state
        # print(state + " " + hidden_state)
        
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
    return signature

        
def initialise_starting_state(P, Q, R, S):
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
    print("Message: " + str(P) + str(Q) + str(R) + str(S))

def measure_time():

    total_time = 0
    for i in range(100):
        start_time = time.time()
        # output = AND_n_qubits([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], [1, 1, 1, 1, 1, 1, 1, 1, 1, 1])
        output = controlled_n_qubit_h([0, 1, 2, 3, 4, 5, 6, 7, 8, 9], [1, 1, 1, 1, 1, 1, 1, 1, 1, 1])
        
        print(str(i) + " " + str(output))
        end_time = time.time()
        total_time = total_time + (end_time - start_time)
    print("Average time: " + str(total_time/100))


def recipient_initialise_starting_state(P, Q, R, S):
    for _ in range(len(circuit.data)):
      circuit.data.pop(0)
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
        
        recreated_signature = recreated_signature + hidden_state
        print(state + " " + hidden_state)
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
  return recreated_signature


start = time.time()
for _ in range(len(circuit.data)):
  circuit.data.pop(0)
STEPS = int(len('1011000001011010110011111011011100111001000010001111011101101100010100100011010010111000110101100011') / 4)
message = '1011000001011010110011111011011100111001000010001111011101101100010100100011010010111000110101100011'
signature = 'x11xx1x1xx01xx10x0x1x1x01x0x10xxxx10x1x10xx1x1x1xx11x00x00xx0xx0xx11xx00x10x1x0x0x0x1xx1xx00x11x0x0xxx11x11xx1x00x1xx0x1x01x1x0xx01x0xx0xx01x1x1xx00x10x0x0x1xx00x0xx1x101xx0xx0x1x1xx10x1x1x0x1x01x1x0xx1x101xx1xx1xx00x10xx01x1xx1x10x0xx0x0x0xx01xx10x1x1x0x1x00xx0x01xx1x00x11xx1x1xx0x10x0xx1x01x0x10xx1x1xxx00x01x0xx10x1x1xx00x1xx0x10x1xxx11xx0100xx10xxx11x0x0x0x1xxx101x0x1x1xxx0010xx0xx10x1xxx11xx01x10x0xx0x0x0xx0110xxx01x0xx10x0xx0x1xx0000xx10xxx11x0x1xx1x1x0x0xx100x0x10xx0xx10x1xxx100x1xx1x1x1x1'
temp_signature = signature
k = int(len(signature)/len(message))
end_index = k*4
recipient_signature = ""
for _ in range(STEPS):
  start_state = message[0:4]
  message = message[4:]
  mess_signature = signature[0:end_index]
  signature = signature[end_index:]
  recipient_signature = recipient_signature + recipient(start_state, mess_signature, '0000')


if(recipient_signature == temp_signature):
  print("ACCEPT")
else:
  print("REJECT")
print(time.time() - start)



