# adaptive-risk-appetite

Install from: https://bitbucket.org/siteks/kilobox and follow the instructions in the readme for a basic install of kilobox

Some users will have to change line 111 of kilobox/Box2D/freeglut/freeglut_internal.h to 

//#    include <X11/extensions/XInput.h>

due to incompatability

--------------------------------------------------
/Box2D
The script acts recursively on itself until a given number of generations are complete.

This example evolves the behaviour of a 64 kilobot swarm in a concentric ring world, other worlds or behaviours can easilly be substituted in. The basic usage involves:

1. Write the script for the behaviour that includes all the variables to be evolved (evokilo1.cpp) and copy this into the gen_kiloscript_3 function in gen_kiloscript_3.py (make sure to use the right syntax, particularly to insert "{" you have to type "{{". - see examples if you are unsure

2. run 40 instances of 

    ./Testbed/Testbed ../kilobox/worlds/concentric\ for\ destination\ choice\ 64.world --seed {3} -g
    python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} 151
    make

where {0} - {2} are the parameters, it makes sense to have these linearly spaced over the space you expect the optimal values to lie in, and {3} can be anything really. It's a random seed, and is only present so that if a particular simulation is giving an odd result, it can be re-run. The central (score-keeping) kilobot keeps a log each time in the beacon.log file. This is used as a simple scoring mechanism and the first part of the gen_kiloscript_3 function writes the final score, along with the variables and random seed into the EA.log file (on a single line).
To watch the simulation, remove the "g", but this slows down the simulation, and removes the automatic cut-off point, so is not advised if you are collecting data.

3. Now that there are 40 bits of data to use, we can let the script automatically evolve and produce graphs etc.

    python3 evolve_params_3.py evolve_params_3 0 && .new\ params

This is all you need to put in. The following is an explnation of what the function executes automatically.
The function takes the highest scoring parameters from the last 40 simualations (logged in EA.log next to their score), evolves them according to the algorithm laid out in the paper (and the script itself) to make a set of 40 new parameters. It then runs these 40 simulations (can be time consuming). Increments the number of 'generations' by 1, then repeats the process again. e.g. the second run will be:

python3 evolve_params_3.py evolve_params_3 1 && .new\ params

it will keep going until it executes

python3 evolve_params_3.py evolve_params_3 20 && .new\ params     (the number of generations -default=20, can be changed in the code)

or reaches a given performance metric. At this point it executes the replicate_3 function. This function pulls out the 10 best performing parameter combinations over all of the generations (out of 40 * 20 = 800), and repeats each one 3 times, recording the scores to a new file.

the repeat_best_6 function then takes the most consistently well performing of these 10 parameter combinations, and repeats it a further 6 times, so that there are 10 simulations for the particualar combination of parameters in total.

The analyse_best_3 function, then takes the best performing of these simulations, passes these through a python script to generate an excel sheet and runs the matlab analysis (draw_paths) on the data, to produce graphs etc.

The Matlab script is quite heavy, and could be streamlined for larger populations. It will need to be modified if not all the graphs etc. are needed/appropriate. An obvious speed-up would result if the excel sheet was skipped. The reader on matlab is slow, as is writing to it from python, the sheet itself can grow to a massive size. CSV would be much faster, or analysing the raw data in matlab directly.


This whole process can take a long time if the population is large. But is automatic, so can be left alone. 


Extra Functions

i. assign_indiv_boldness.py

is used to seed a boldness distribution in a population from a given file, these may be copied from another simulation analysis, or created. The output can be copied into the evokilo.cpp preamble to set the boldnesses. This allows the user to specify starting conditions, which is desirable if the population is small, or if an uneven starting condition is to of interest.  

ii. get_energies.m
This script works in  a similar way to draw_paths.m. It plots the information associated with the distance travelled, and related this to the boldness of an individual kilobot, and the swarm at large. In general, we found that bolder robots tended to travel much further, so minimising boldness provides a feasible way to minimise energy expenditure.

iii. RWMM.m
this function provides a way to approximate the probability density function of the kilobots given the size of the arena, the number of kilobots, and the severity of collisions. It is based on the Random Waypoint Model.

iv. RWMM_pred.m
this is similar to the RWMM function, but quantifies the prediction into a histogram so that it can be compared directly to simulated or otherwise collected data.

--------------------------------------------------
/world

The .world files are essentially made up of two parts, the part setting up the environment (shapes, lights, arena...) and the part setting up the Kilobots. There is a python script to generate each half. Code could easilly be written to join the two halves and generate a .world file in a way that may be more user friendly. The time for which the simulation is run (when the

gen_array.py
This script generates the positions for a given number of robots. It can be called in three ways from terminal. The resulting .txt file can be copied into a world file to give the initial layout of the Kilobots. I will give an example and then an explanation for each one

    python3 gen_array.py gen_array 30

"gen_array" generates a square of Kilobots to the size of the largest square number that is smaller than the argument given. Here, it will generate a square of 25 Kilobots in the 'array.txt' file, ready for copying into the .world file

    python3 gen_array.py gen_line 20

"gen_line" generates a line of kilobots separated by 5mm (this can be changed in the script) and outputs this to the 'line.txt' file. The code is currently set up to draw 3 vertical lines of Kilobots, this can be changed quite easily by editing the python script.

    python3 gen_array.py gen_ring 64 3

This was the code used in all reported experiments. This example generates 64 robots evenly spaced in a ring at a radius of 3 and writes this to the 'ring.txt' file

This script generates the environment for the Kilobots

    python3 generate_concentric_emmental.py concentric 10
Generates 10 concentric rings with linearly increasing radii from 0.2 - 0.9m.

    python3 generate_concentric_emmental.py concentric_size 10
Generates 10 concentric rings with a linearly increasing area

    python3 generate_concentric_emmental.py emmental 10 0.1 0.05
This is to ismulate an environment with patches of dangerous regions. This code generates 10 randomly placed circles of radius 0.1 and chance of dying 5% with each clock tick.

    python3 generate_concentric_emmental.py grid_world 10
Generates a 10x10 patchwork grid-world. the boundaries are necessarilly different in this geometry, as there are now 2 gradients. In order to do this there are 5 colours 'projected', and the kilobots can deduce which direction they are moving in by how much the modulo of the number associated with their region changes. e.g. and even number change means they are travelling horizontally, and positive means it is to the right.

    python3 generate_concentric_emmental.py pipe 10
Generates a thin environment where the kilobots cannot move around each other. The '10' variable determines how frequent the regions change, and so how accurately the kilobots know their position.

----------------------------------------------------- 

EA.log is the log file produced by the EA when evovlving the behaviours for Kilobox. The first columns represent the a, b, c and random seed that were tested. The final column is the score achieved by that set of inputs.

In the data uploaded to Github, the first runs, where the behaviour is not written were carried out using the en-route behaviour, as this was the first behaviour that the EA was used to optimise. Some of the data is repeated, so that it could be used as input to for the first generation of another run of the EA.
