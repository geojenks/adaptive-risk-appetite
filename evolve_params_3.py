#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Aug  2 18:10:28 2019

@author: jorge
"""

import random
import fire

# for gens = 10 I have been starting on generation 11. 10 generations seems to settle on a pretty steady performance. Although with enough gens perhaps this could improve even further - experiment with 16 bots over 100 generations if time is there

# I have been putting "python3 evolve_params_3.py evolve_params_3 11 && ./new\ params" at the bottom of the existing new params document and changed the worlds when increasing the population

# Started 64 at 20:32 5/9

class evolve_params_3(object):
    """
    This function evolves the parameters to find good combinations over many generations.
    After x generations, the function passes to replicate_3, which looks for the best 10 performances over the x generations,
    and repeats each one 3 times.
    The best performer out of these 30 simulations is repeated a further 6 times, so that there are 10 datapoints for
    the inputs, then the best of these 10 is chosen by analyse_best_3 to have plots etc. made by matlab
    
    """
    def evolve_params_3(self, generation):
        generation = generation + 1
        #append latest score
        with open('beacon.log', 'r') as readFile:
            doc = readFile.read()
            end = doc[-30:-2]
            end = end.split(",")
            score = end[-1].strip()
        # open the file, append the previous score
        with open('EA.log', 'r') as readFile:
            lines = readFile.read()
            if lines[-1] == '\n':
                lines = lines[:-1]
            lines = lines + score
        #write the file with the latest score added
        with open('EA.log', 'w') as writeFile:
            writeFile.write(lines)
        with open("EA.log", 'r') as readFile:
            As = []
            Bs = []
            Cs = []
            Scores = []
            lines = readFile.readlines()
            for i in lines[-40:]:
                i = i.split(',')
                As.append(i[0])
                Bs.append(i[1])
                Cs.append(i[2])
                Scores.append(i[4])
            best = sorted(Scores)[-10:]
            Astouse = []
            Bstouse = []
            Cstouse = []
            #copy the best with jitter up to 0.1 - also ensure it is positive
            for score in best:
                ind = Scores.index(score)
                Astouse.append(abs(float(As[ind])-0.1+random.random()/5))
                Bstouse.append(abs(float(Bs[ind])-0.1+random.random()/5))
                Cstouse.append(abs(float(Cs[ind])-0.1+random.random()/5))
            #split them into 2 "genes"
            Agenes1 = []
            Agenes2 = []
            for i in Astouse:
                factor = random.random()
                Agenes1.append(factor * float(i))
                Agenes2.append(float(i)/(factor * float(i)))
            Bgenes1 = []
            Bgenes2 = []
            for i in Bstouse:
                factor = random.random()
                Bgenes1.append(factor * float(i))
                Bgenes2.append(float(i)/(factor * float(i)))
            Cgenes1 = []
            Cgenes2 = []
            for i in Cstouse:
                factor = random.random()
                Cgenes1.append(factor * float(i))
                Cgenes2.append(float(i)/(factor * float(i)))
            #mix the phenomes
            for i in range(0,10):
                Astouse.append(random.choice(Astouse[0:4]))# change 4 to 9
                Bstouse.append(random.choice(Bstouse[0:4]))
                Cstouse.append(random.choice(Cstouse[0:4]))
            #mix the genomes for new values
            for i in range(0,20):
                Astouse.append(random.choice(Agenes1)*random.choice(Agenes2))
                Bstouse.append(random.choice(Bgenes1)*random.choice(Bgenes2))
                Cstouse.append(random.choice(Cgenes1)*random.choice(Cgenes2))
        with open("new params", 'w') as writeFile:
            lines = ""
            for i in range(len(Astouse)):
                lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g\n".format(str(Astouse[i]), str(Bstouse[i]), str(Cstouse[i]), str(random.randint(1,1000)))
            #if the best isn't good enough, evolve again (1 generation lag)
            #or stop after 20 generations
            #if (float(best[-1]) < 90000) & (generation < 20):
            if generation < 20:
                #with open('EA.log', 'r') as readFile:
                #    lines2 = readFile.read() + "Generation {0}\n".format(generation)
                #with open('EA.log', 'w') as writeFile2:
                #    writeFile2.write(lines2)
                lines = lines + "\npython3 evolve_params_3.py evolve_params_3 {0} && ./new\\ params\n\n".format(generation)
            else:
                print("final generation")
                lines = lines + "\npython3 evolve_params_3.py replicate_3 && ./EA\ repeat\ runs && python3 evolve_params_3.py repeat_best_6 && ./EA\ best\ to\ redo\ 6\ times && python3 evolve_params_3.py analyse_best_3 && ./EA\ best\ to\ analyse\n\n"
            writeFile.write(lines)
            # Can now run "./new\ params" which should keep going until 90000 is reached


    """
    This function reproduces the top 10 performances 3 times each in an attempt to find the best combination of inputs.
    It is called at the end of evolve_params_3
    """
    def replicate_3(self):
        gens = 3
        # open the file, remove blank lines
        with open('EA.log', 'r') as readFile:
            lines = readFile.read()
            if lines[-1] == '\n':
                lines = lines[:-1]
            lines = lines
        with open('EA.log', 'w') as writeFile:
            writeFile.write(lines)
        with open("EA.log", 'r') as readFile:
            As = []
            Bs = []
            Cs = []
            Scores = []
            lines = readFile.readlines()
            for i in lines[-40*gens:]:#work this out based on number of gens (starting at gen 12 currently)
                i = i.split(',')
                As.append(i[0])
                Bs.append(i[1])
                Cs.append(i[2])
                if len(i[4]) > 7: # at the end of a generation, scores get written twice, back to back. so the string needs to be halved
                    sc = i[4][0:int(len(i[4])/2)]
                else:
                    sc = i[4]
                Scores.append(sc)
            best = sorted(Scores)[-10:]
            Astouse = []
            Bstouse = []
            Cstouse = []
            #copy the best 10
            for score in best:
                ind = Scores.index(score)
                Astouse.append(abs(float(As[ind])))
                Bstouse.append(abs(float(Bs[ind])))
                Cstouse.append(abs(float(Cs[ind])))
            #for i in range(0,10):
            #    Astouse.append(Astouse[i])
            #    Bstouse.append(Bstouse[i])
            #    Cstouse.append(Cstouse[i])
        with open("EA repeat runs", 'w') as writeFile:
            lines = ""
            for i in range(len(Astouse)): #add 3 experiments for each set of parameters
                lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g\n".format(str(Astouse[i]), str(Bstouse[i]), str(Cstouse[i]), str(random.randint(1,1000)))
                lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g\n".format(str(Astouse[i]), str(Bstouse[i]), str(Cstouse[i]), str(random.randint(1,1000)))
                lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g\n".format(str(Astouse[i]), str(Bstouse[i]), str(Cstouse[i]), str(random.randint(1,1000)))
            #if the best isn't good enough, evolve again (1 generation lag)
            #or stop after 20 generations
            #if (float(best[-1]) < 90000) & (generation < 20):
            #with open('EA.log', 'r') as readFile:
            #    lines2 = readFile.read() + "Generation {0}\n".format(generation)
            #with open('EA.log', 'w') as writeFile2:
            #    writeFile2.write(lines2)
            lines = lines + "\n\n"
            writeFile.write(lines)
            # Can now run "./EA\ repeat\ runs" which should keep going until 90000 is reached

    """
    This function takes the combination producing the best of the 30 simulations, appends the 4 already executed simulations,
    and produces a script to repeat it a further 6 times to produce 10 datapoints in total
    """
    def repeat_best_6(self):
        # open the file, remove blank lines
        gens = 3
        with open('beacon.log', 'r') as readFile:
            doc = readFile.read()
            end = doc[-30:-2]
            end = end.split(",")
            score = end[-1].strip()
        # open the file, append the previous score
        with open('EA.log', 'r') as readFile:
            lines = readFile.read()
            if lines[-1] == '\n':
                lines = lines[:-1]
            lines = lines + score + "\n"
        with open('EA.log', 'w') as writeFile:
            writeFile.write(lines)
        with open("EA.log", 'r') as readFile:
            As = []
            Bs = []
            Cs = []
            Scores = []
            lines = readFile.readlines()
            for i in lines[-30:]: #all of the 'reproduced' experiments 
                i = i.split(',')
                As.append(i[0])
                Bs.append(i[1])
                Cs.append(i[2])
                Scores.append(int(i[4]))
            best = sorted(Scores)#[-1:]
            tally = []
            order = []
            print(best[-1])
            for i in best[::-1]:
                for j in lines[-30:]:
                    #print(j)
                    j = j.split(',')
                    if int(j[4]) == i:
                        if float(j[0])*float(j[1])*float(j[2]) in tally: # the first combination to rank again after being added to the tall is taken as the best
                            order = order + [j[0], j[1], j[2]]
                        else:
                            tally = tally + [float(j[0])*float(j[1])*float(j[2])] # multiplying the inputs will act as an identifying label
            #append the last 4 experiments of the best performer to use as datapoints
            #for i in lines[-30:]: #all of the 'reproduced' experiments
            #    j = i.split(',')
            #    if (order[0] == j[0]) & (order[1] == j[1]) & (order[2] == j[2]):
            #        if lines[-1][-1] != '\n':
            #            lines = lines + ["\n"]
            #        lines = lines + [i]
            for i in lines[-40*gens-30:]: #all of the initial and 'reproduced' experiments
                j = i.split(',')
                if (order[0] == j[0]) & (order[1] == j[1]) & (order[2] == j[2]):
                    lines = lines + [i]
            writepeice = ""
            for line in lines:
                writepeice = writepeice + line
            with open('EA.log', 'w') as writeFile:
                writeFile.write(writepeice)
            #^^ no need to split these up really

            #Astouse = [order[0]]
            #Bstouse = [order[1]]
            #Cstouse = [order[2]]
            #copy the best 10
            #for score in best:
            #    ind = Scores.index(score)
            #    Astouse.append(abs(float(As[ind])))
            #    Bstouse.append(abs(float(Bs[ind])))
            #    Cstouse.append(abs(float(Cs[ind])))
            #for i in range(0,1):
            #    Astouse.append(Astouse[i])
            #    Bstouse.append(Bstouse[i])
            #    Cstouse.append(Cstouse[i])
            # I've missed the step of reproducing the best looking one 6 times to have 10 datasets!
        with open("EA best to redo 6 times", 'w') as writeFile:
            lines = ""
# uncomment these for the function to work!
            #lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g \n".format(str(order[0]), str(order[1]), str(order[2]), str(random.randint(1,1000)))
            #lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g \n".format(str(order[0]), str(order[1]), str(order[2]), str(random.randint(1,1000)))
            #lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g \n".format(str(order[0]), str(order[1]), str(order[2]), str(random.randint(1,1000)))
            #lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g \n".format(str(order[0]), str(order[1]), str(order[2]), str(random.randint(1,1000)))
            #lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g \n".format(str(order[0]), str(order[1]), str(order[2]), str(random.randint(1,1000)))
            #lines = lines + "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --seed {3} -g \n".format(str(order[0]), str(order[1]), str(order[2]), str(random.randint(1,1000)))

            #if the best isn't good enough, evolve again (1 generation lag)
            #or stop after 20 generations
            #if (float(best[-1]) < 90000) & (generation < 20):
            #with open('EA.log', 'r') as readFile:
            #    lines2 = readFile.read() + "Generation {0}\n".format(generation)
            #with open('EA.log', 'w') as writeFile2:
            #    writeFile2.write(lines2)
            writeFile.write(lines)
            # Can now run "./EA\ best\ to\ analyse" which should keep going until 90000 is reached

    """
    This function processes the best result of the 10 to produce matlab plots etc. It is called at the end of replicate_3
    """
    def analyse_best_3(self):
#append latest score
        with open('beacon.log', 'r') as readFile:
            doc = readFile.read()
            end = doc[-30:-2]
            end = end.split(",")
            score = end[-1].strip()
        # open the file, remove blank lines, add latest score
        with open('EA.log', 'r') as readFile:
            lines = readFile.read()
            if lines[-1] == '\n':
                lines = lines[:-1]
            lines = lines + score
        with open('EA.log', 'w') as writeFile:
            writeFile.write(lines)
        with open("EA.log", 'r') as readFile:
            As = []
            Bs = []
            Cs = []
            Scores = []
            lines = readFile.readlines()
            #for i in lines[-10:]: #all of the 'reproduced' experiments 
            for i in lines[-30:]: #changed to 30 for a single analysis
                i = i.split(',')
                As.append(i[0])
                Bs.append(i[1])
                Cs.append(i[2])
                if len(i[4]) > 7: # at the end of a generation, scores get written twice, back to back. so the string needs to be halved
                    #print(len(i[4]))
                    #print(len(i[4])/2)
                    #print(i[4])
                    sc = i[4][0:int(len(i[4])/2)]
                    #print(sc)
                else:
                    sc = i[4]
                Scores.append(sc)
            best = sorted(Scores)#[-1:]
            #print(best)
            order = []
            for i in best:
                #for j in lines[-10:]:
                for j in lines[-30:]: #changed to 30 for a single analysis
                    j = j.split(',')
                    if len(i[4]) > 7: # at the end of a generation, scores get written twice, back to back. so the string needs to be halved
                        #print(len(i[4]))
                        #print(len(i[4])/2)
                        #print(i[4])
                        sc = i[4][1:int(len(i[4])/2)]
                        #print(sc)
                    else:
                        sc = i[4][1:-1]
                    if best[-1] in j[4]:
                        order = [j[0], j[1], j[2], j[3]]
            #Astouse = [order[0]]
            #Bstouse = [order[1]]
            #Cstouse = [order[2]]
            #copy the best 10
            #for score in best:
            #    ind = Scores.index(score)
            #    Astouse.append(abs(float(As[ind])))
            #    Bstouse.append(abs(float(Bs[ind])))
            #    Cstouse.append(abs(float(Cs[ind])))
            #for i in range(0,1):
            #    Astouse.append(Astouse[i])
            #    Bstouse.append(Bstouse[i])
            #    Cstouse.append(Cstouse[i])
            # I've missed the step of reproducing the best looking one 6 times to have 10 datasets!
        with open("EA best to analyse", 'w') as writeFile:
            lines = "python3 gen_kiloscript_3.py gen_kiloscript_3 {0} {1} {2} {3}\nmake\n./Testbed/Testbed ../kilobox/worlds/concentric\\ for\\ destination\\ choice\ 64.world --args=\"log test.log\" --seed {3} -g".format(str(order[0]), str(order[1]), str(order[2]), str(order[3])) + "&& python3 analyse_log.py analyse && chmod +x executematlab && ./executematlab\n\n"
            #if the best isn't good enough, evolve again (1 generation lag)
            #or stop after 20 generations
            #if (float(best[-1]) < 90000) & (generation < 20):
            #with open('EA.log', 'r') as readFile:
            #    lines2 = readFile.read() + "Generation {0}\n".format(generation)
            #with open('EA.log', 'w') as writeFile2:
            #    writeFile2.write(lines2)
            writeFile.write(lines)
            # Can now run "./EA\ best\ to\ analyse" which should keep going until 90000 is reached

if __name__ == '__main__':
  fire.Fire(evolve_params_3)
