#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jul 31 16:29:38 2019

@author: jorge
"""

import fire

class assign_indiv_boldness(object):
    def assign(self):        # get the boldnesses needed for individual assignment
        with open('boldnesses', 'r') as readFile:
            boldies = readFile.readlines()
        lines = ""
        for i in range(len(boldies)):
            lines = lines + 'if ((float)kilo_uid == {0}) \n {{bold = {1};}}\n'.format(str(i+1), str(boldies[i]))
        with open('Box2D/Testbed/Tests/evokilo1.cpp', 'r') as readfile:
            text = readfile.read()
            index = text.find('kilo_message_tx         = (message_tx_t)&Evokilo8::tx_message;')
        text = text[:index] + lines + text[index:]
        with open('Box2D/Testbed/Tests/evokilo1.cpp', 'w') as writeFile:
            writeFile.write(text)

if __name__ == '__main__':
  fire.Fire(assign_indiv_boldness)
