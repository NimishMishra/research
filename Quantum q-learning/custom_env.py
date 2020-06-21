import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
import pickle
from matplotlib import style
import time

style.use("ggplot")

# DECIDE HOW TO TUNE THESE PARAMETERS
# Refer to video 3 for a primer

SIZE = 10 # size of the grid
EPISODES = 20_000 # episodes to run
MOVE_PENALTY = 0.1
ENEMY_PENALTY = 300
FOOD_REWARD = 100
SHOULD_EXPLORE = False

epsilon = 1
EPS_DECAY = 0.9998
SHOW_EVERY = 3000

deterministic_actions = 0
non_deterministic_actions = 0

# upload existing q tables here whenever required
start_q_table = None

LEARNING_RATE = 0.1
DISCOUNT = 0.95

PLAYER_N = 1
FOOD_N = 2
ENEMY_N = 3

# colors of the three things to be used
# BGR
color_dict = {
    1: (255, 175, 0),
    2: (0, 255, 0),
    3: (0, 0, 255)}


#################################################################################

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
    return counts[::-1]


create_circuits() # create the circuits

#################################################################################





class Blob:
    
    def __init__(self):
        
        # random spawning
        self.x = np.random.randint(0, SIZE)
        self.y = np.random.randint(0, SIZE)

    def __str__(self):
        return f"{self.x}, {self.y}"

    # overloading the - operator
    def __sub__(self, other):
        return (self.x - other.x, self.y - other.y)
    
    def action(self, choice):
        # 8 actions

        if(choice == 0):
            action_choice = "000"

        elif(choice == 1):
            action_choice = "001"

        elif(choice == 2):
            action_choice = "010"

        elif(choice == 3):
            action_choice = "011"

        elif(choice == 4):
            action_choice = "100"

        elif(choice == 5):
            action_choice = "101"

        elif(choice == 6):
            action_choice = "110"

        elif(choice == 7):
            action_choice = "111"

        action = execute_circuit(str(action_choice))
        if(action == "110"):
            # upper right
            self.move(x=1, y=1)

        elif(action == "011"):
            # lower left
            self.move(x=-1, y=-1)

        elif(action == "001"):
            # upper left
            self.move(x=-1, y=1)

        elif(action == "100"):
            # lower right
            self.move(x=1, y=-1)

        elif(action == "010"):
            # right
            self.move(x=1, y=0)

        elif(action == "101"):
            # left
            self.move(x=-1, y=0)

        elif(action == "000"):
            # up
            self.move(x=0, y=1)

        elif(action == "111"):
            # down
            self.move(x=0, y=-1)
        

    def move(self, x=False, y=False):
        
        # if x value is not provided. Random movement
        if not x:
            # choose -1, 0, 1
            self.x += np.random.randint(-1, 2)
        
        else:
            self.x += x
        
        # if x value is not provided. Random movement
        if not y:
            # choose -1, 0, 1
            self.y += np.random.randint(-1, 2)
        
        else:
            self.y += y

        # handling the wall

        if self.x < 0:
            self.x = 0
        
        elif self.x > SIZE - 1:
            self.x = SIZE - 1

        if self.y < 0:
            self.y = 0
        
        elif self.y > SIZE - 1:
            self.y = SIZE - 1

if start_q_table is None:
    # dictionary
    q_table = {}
    
    # build a table relative to the enemy and 
    # the food

    for x1 in range (-SIZE + 1, SIZE):
        for y1 in range (-SIZE + 1, SIZE):
            for x2 in range (-SIZE + 1, SIZE):
                for y2 in range (-SIZE + 1, SIZE):

                    # there are 8 actions, so we choose a list of 4 random values corresponding to this observation space
                    q_table[( (x1, y1), (x2, y2) )] = [np.random.uniform(-5, 0) for i in range(8)]


else:
    with open(start_q_table, "rb") as f:
        q_table = pickle.load(f)

food_encountered_times = 0
enemy_encountered_times = 0
episode_rewards = []
for episode in range(EPISODES):
    
    deterministic_actions = 0
    non_deterministic_actions = 0

    player = Blob()
    food   = Blob()
    enemy  = Blob()

    if (episode % SHOW_EVERY == 0):
        print(f"on episode {episode}, epsilon: {epsilon}")
        print(f"{SHOW_EVERY} episode mean {np.mean(episode_rewards[-SHOW_EVERY: ])}")
        show = True
    else:
        show = False
    
    # start with 0
    episode_reward = 0
    STEPS = 200
    for step in range(STEPS):

        # make an initial observation of the environment
        observation = (player - food, player - enemy)
        
        if(SHOULD_EXPLORE):
            # should the agent explore in the beginning, based on epsilon
            if (np.random.random() > epsilon):
                action = np.argmax(q_table[observation])
            else:
                # number of actions 
                action = np.random.randint(0, 8)
        else:
            # no exploration
            action = np.argmax(q_table[observation])

        if(action % 2 == 0):
            deterministic_actions = deterministic_actions + 1
        else:
            non_deterministic_actions = non_deterministic_actions + 1

        # take some action
        player.action(action)

        # enemy encountered
        if(player.x == enemy.x and player.y == enemy.y):
            enemy_encountered_times = enemy_encountered_times + 1
            reward = - ENEMY_PENALTY
        # food encountered
        elif(player.x == food.x and player.y == food.y):
            print("Episode: " + str(episode) + " food encountered with non-deterministic actions: " + str( (non_deterministic_actions * 100)/(non_deterministic_actions + deterministic_actions)) + " %" )
            food_encountered_times = food_encountered_times + 1
            reward = FOOD_REWARD
        
        # simulates a timer in a way. In a way, more number of steps
        # have higher penalty than less number of steps
        else:
            reward = -MOVE_PENALTY
        
        # observation after making the move
        new_observation = (player-food, player - enemy)
        # what action are we to take in future
        max_future_q = np.max(q_table[new_observation])
        
        # main q-learning update
        current_q = q_table[observation][action] 
        if (reward == FOOD_REWARD):
            new_q = FOOD_REWARD
        
        # elif(reward == - ENEMY_PENALTY):
        #     new_q = - ENEMY_PENALTY

        else:
            new_q = (1 - LEARNING_RATE) * current_q + LEARNING_RATE*(reward + DISCOUNT * max_future_q)
        # update the q-value
        q_table[observation][action] = new_q


        # build the environment
        if show:
            env = np.zeros( (SIZE, SIZE, 3), dtype=np.uint8)
            env[food.x][food.y] = color_dict[FOOD_N]
            env[player.x][player.y] = color_dict[PLAYER_N]
            env[enemy.x][enemy.y] = color_dict[ENEMY_N]

            image = Image.fromarray(env, "RGB")
            image = image.resize((300, 300))
            cv2.imshow("", np.array(image))
            if (reward == FOOD_REWARD or reward == -ENEMY_PENALTY):
                if (cv2.waitKey(500) & 0xFF == ord("q")):
                    break
            else:
                if (cv2.waitKey(1) & 0xFF == ord("q")):
                    break
        episode_reward += reward
        if (reward == FOOD_REWARD or reward == -ENEMY_PENALTY):
            break
        
        # moving both the food and enemy

        # food.move()
        # enemy.move()

        #################
        
    episode_rewards.append(episode_reward)
    epsilon *= EPS_DECAY

print("Food encountered: " + str(food_encountered_times) + " times")
print("Enemy encountered: " + str(enemy_encountered_times) + " times")
# statistics for later use
moving_avg = np.convolve(episode_rewards, np.ones( (SHOW_EVERY, )) / SHOW_EVERY,  mode="valid")

plt.plot( [i for i in range(len(moving_avg))], moving_avg)
plt.ylabel(f"Reward every {SHOW_EVERY} episodes")
plt.xlabel("Episode #")
plt.show()

with open(f"qtable {int(time.time())}.pickle", "wb") as f:
    pickle.dump(q_table, f)