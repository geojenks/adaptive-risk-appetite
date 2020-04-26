# -*- coding: utf-8 -*-
"""
Created on Sun Apr 28 14:37:32 2019

@author: jorge
"""
import random
import fire
import numpy as np

class make_world(object):
    # generates a number of circles between the radius 0.2 and 0.9. For kilobox simulations
    def concentric(self, num):
        with open('concentric_regions.txt', 'w') as writeFile:
            lines = ""
            ident = 1
            for i in range(1, 1+num):
                # increase the radius by desired increment
                # inc = 0.2 + i*(1.3/num)
                inc = i*(1.2/num)
                # append new line with this increment, and 3 or 0 alternatively
                lines = ('region( circle [0 0 {0} {1}] )').format(str(inc), ident) + "\n" + lines
                # alternate between 1, 2, and 3
                ident = 1+(ident)%3#3*((1+ident)%2)
            writeFile.write(lines)

    # generates concentric rings, each with the same area
    def concentric_size(self, num):
        with open('concentric_regions.txt', 'w') as writeFile:
            lines = ""
            ident = 1
            for i in range(1, 1+num):
                # increase the radius by desired increment
                # inc = 0.2 + i*(1.3/num)
                final_radius = 3
                inc = np.sqrt(((i*np.pi*pow(final_radius,2))/num)/np.pi)
                # append new line with this increment, and 3 or 0 alternatively
                lines = ('region( circle [0 0 {0} {1}] )') + "\n" + lines
                # alternate between 1, 2, and 3
                ident = 1+(ident)%3#3*((1+ident)%2)
            writeFile.write(lines)
    def emmental(self, num= 25, nest_rad = 0.2, danger_size = 0.05, buffer_size = 0.05):

        with open('emmental_regions.txt', 'w') as writeFile:
            lines = ""
            edge = nest_rad + danger_size + buffer_size
            for i in range(1, 1+num):
                x = nest_rad/10
                y = nest_rad/10
                while np.sqrt((x-0.1)*(x-0.1) + (y-0.1)*(y-0.1)) < edge:
                    x = random.uniform(0,1.5)*pow(-1,random.randint(1,2))
                    y = random.uniform(0,1)*pow(-1,random.randint(1,2))
                # append new line with this increment, and 3 or 0 alternatively
                lines = ('region( circle [{0} {1} {2} 2] )').format(str(x), str(y), str(danger_size)) + "\n\n" + lines
                lines = ('region( circle [{0} {1} {2} 0] )').format(str(x), str(y), str(danger_size + buffer_size)) + "\n" + lines
            writeFile.write("region( rectangle [0 0 3 2 3] )\n\n" + lines + "region( circle [0 0 {0} 1] )".format(str(nest_rad)))

    def grid_world(self, num):
#        if num % 5 != 3:
#            print("number must have remainder of 3 when divided by 5")
            
        with open('gridworld.txt', 'w') as writeFile:
            lines = ""
            identx = 1
            for i in range(num):
                incx = 3/num
                incy = 2/num
                xcentre = -1.5+i*incx+incx/2
                identy = identx
                for j in range(num):
                    ycentre = 1-j*incy-incy/2
                    # append new line with this increment, and 3 or 0 alternatively
                    lines = ('region( rectangle [{0} {1} {2} {3} {4}] )'.format(str(xcentre), str(ycentre), str(incx), str(incy), str(identy))) + "\n" + lines
                    # alternate between 1, 2, and 3
                    identy = (2+identy)%5
                identx = (1+identy)%5
            writeFile.write(lines)

    def pipe(self, num):
        with open('pipe_regions.txt', 'w') as writeFile:
            lines = ""
            ident = 1
            for i in range(1, 1+num):
                # increase the radius by desired increment
                # inc = 0.2 + i*(1.3/num)
                inc = i*(4/num)
                # append new line with this increment, and 3 or 0 alternatively
                lines = ('region( rectangle [{0} 0 0.2 0.2 {1}] )').format(str(-2+inc), ident) + "\n" + lines
                # alternate between 1, 2, and 3
                ident = 1+(ident-2)%3#3*((1+ident)%2)
            writeFile.write(lines)

if __name__ == '__main__':
  fire.Fire(make_world)

