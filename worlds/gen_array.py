#!/usr/bin/env python

import fire
import sys
import math
import random
import numpy as np

class gen_array(object):
    # Generate a regular array of randomly posed kilobots.
    # Uses nearest square number below first arg
    def gen_array(self, num):
        per_side = int(math.sqrt(num))
        separation = 0.04
        count = 0
        with open('array.txt', 'w') as writeFile:
            lines = ""
            prelines = ""
            for i in range(per_side):
                for j in range(per_side):
                    x = (i - (per_side - 1) / 2.0) * separation
                    y = (j - (per_side - 1) / 2.0) * separation
                    a = random.uniform(-180,180)
                    count += 1
                    prelines = prelines + ("define r{0} position (ctrl \"evokilo8\")").format(str(i*per_side+j)) + "\n"
                    lines = lines  + ('r{0}( pose [ {1} {2} 0 {3} ])').format(str(i*per_side+j), str(x), str(y), str(a)) + "\n"
                    #print ('r0( pose [ %f %f 0 %f ])').format(str(x), str(y), str(a)) + "\n" + lines
            writeFile.write(prelines + lines)

        #print 'Total generated %d' % count


    def gen_line(self, num):
        separation = 0.005
        count = 0
        with open('line.txt', 'w') as writeFile:
            lines = ""
            prelines = ""
            for i in range(num):
                x = -1.2 - 0.03*float(int(i/(num/3)))
                y = ((i*3)%num - (num - 1) / 2.0) * separation
                a = random.uniform(-180,180)
                count += 1
                prelines = prelines + ("define r{0} position (ctrl \"evokilo8\")").format(str(i)) + "\n"
                lines = lines  + ('r{0}( pose [ {1} {2} 0 {3} ])').format(str(i), str(x), str(y), str(a)) + "\n"
                #print ('r0( pose [ %f %f 0 %f ])').format(str(x), str(y), str(a)) + "\n" + lines
            writeFile.write(prelines + lines)

        #print 'Total generated %d' % count

    # rad is radius
    def gen_ring(self, num, rad):
        separation = 0.04
        count = 0
        with open('ring.txt', 'w') as writeFile:
            lines = ""
            prelines = ""
            for i in range(num):
                x = rad*np.sin(i*(2*np.pi - 2*np.pi/num)/(num-1))
                y = rad*np.cos(i*(2*np.pi - 2*np.pi/num)/(num-1))
                a = random.uniform(-180,180)
                count += 1
                prelines = prelines + ("define r{0} position (ctrl \"evokilo8\")").format(str(i)) + "\n"
                lines = lines  + ('r{0}( pose [ {1} {2} 0 {3} ])').format(str(i), str(x), str(y), str(a)) + "\n"
                #print ('r0( pose [ %f %f 0 %f ])').format(str(x), str(y), str(a)) + "\n" + lines
            writeFile.write(prelines + lines)

        #print 'Total generated %d' % count
if __name__ == '__main__':
  fire.Fire(gen_array)
