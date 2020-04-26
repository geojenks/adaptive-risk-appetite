#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jul 31 16:29:38 2019

@author: jorge
"""

import fire


class gen_kiloscript_3(object):
    """
    Generates the code to be compiled, given the variables. Because the
    simulation is run outside of python, we have to log the previous score, 
    rather than the current one.
    '3' generates kilobots that change target en-route
    """
    def gen_kiloscript_3(self, a, b, c, seed):
        
        # first of all, get the score from last time
        with open('beacon.log', 'r') as readFile:
            doc = readFile.read()
            end = doc[-30:-2]
            end = end.split(",")
            score = end[-1].strip()
        # open the file, append the previous score, and the variables we are about to test
        with open('EA.log', 'r') as readFile:
            lines = readFile.read()
            if lines[-1][-1] == '\n':
                lines = lines[:-1]
            lines = lines + score + '\n{0}, {1}, {2}, {3}, '.format(str(a), str(b), str(c), str(seed))
        # write to file
        with open('EA.log', 'w') as writeFile:
            writeFile.write(lines)
        
        with open('Box2D/Testbed/Tests/evokilo1.cpp', 'w') as writeFile:
            lines = '#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
#include <vector>\n\
#include <iostream>\n\
#include <string>\n\
#include <sstream>\n\
#include <algorithm>\n\
#include <iterator>\n\
\n\
#include "kilolib.h"\n\
//#include "kilolibc.c"\n\
#include "kilolibc.h"\n\
#include "evokilo1.h"\n\
\n\
\n\
#define NEST 0\n\
#define FOOD 3\n\
\n\
using namespace Kilolib;\n\
\n\
FILE *Evokilo1::lfp         = NULL;\n\
FILE *Evokilo5::lfp         = NULL;\n\
FILE *Evokilo6::lfp         = NULL;\n\
FILE *Evokilo7::lfp         = NULL;\n\
FILE *Evokilo8::lfp         = NULL;\n\
FILE *Evokilo9::lfp         = NULL;\n\
FILE *Evokilo10::lfp         = NULL;\n\
FILE *Evokilo11::lfp         = NULL;\n\
FILE *Evokilo12::lfp         = NULL;\n\
\n\
\n\
void Evokilo5::setup(){{}}\n\
void Evokilo5::loop(){{}}\n\
void Evokilo6::setup(){{}}\n\
void Evokilo6::loop(){{}}\n\
void Evokilo7::setup(){{}}\n\
void Evokilo7::loop(){{}}\n\
void Evokilo9::setup(){{}}\n\
void Evokilo9::loop(){{}}\n\
void Evokilo10::setup(){{}}\n\
void Evokilo10::loop(){{}}\n\
void Evokilo11::setup(){{}}\n\
void Evokilo11::loop(){{}}\n\
void Evokilo12::setup(){{}}\n\
void Evokilo12::loop(){{}}\n\
\n\
void Evokilo1::setup()\n\
{{\n\
    kilo_message_rx         = (message_rx_t)&Evokilo1::message_rx;\n\
}}\n\
\n\
\n\
void Evokilo1::loop()\n\
{{\n\
    int region;\n\
    if (kilo_ticks > last_update + 16)\n\
    {{\n\
        last_update = kilo_ticks;\n\
        updates++;\n\
        region = get_environment();\n\
        if (region != last_region)\n\
            //direction is simply towards or away from centre\n\
            {{direction = region - last_region; // make a version where the direction is chosen, rather that a target distance\n\
            if (direction == -2) {{direction = 1;}} // cycling through - this is still "ascent"\n\
            if (direction == 2) {{direction = -1;}} // cycling through - this is "descent"\n\
            distance += direction;\n\
            if (direction == -targetdirection) {{region=4; turning = 1;}} // if heading in the wrong direction, turn around\n\
                else\n\
                    if (turning == 1) {{lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}}}} // undo hard turn after it has been executed\n\
        if (distance == 0){{\n\
        lm = 0;\n\
        rm = 0;\n\
        }}\n\
        else {{\n\
        if (turning == 1){{region = 4;}}\n\
        int d = region;\n\
        region = get_environment();\n\
        switch(d)\n\
        {{\n\
            case(0):\n\
				// Random walk in nest\n\
				//set_motors(70, 60);\n\
				if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(1):\n\
                //set_motors(70, 70);\n\
                if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(2):\n\
				//set_motors(70, 70);\n\
				if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(3):\n\
				//set_motors(70, 70);\n\
				if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(4):\n\
                if (lm > rm) {{lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}}\n\
                else {{lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}}\n\
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)\n\
				//std::cout << "backwards!";\n\
				region      = get_environment();\n\
                break;\n\
            case(5): //an attempt to break out of getting stuck in circles\n\
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;\n\
				region      = get_environment();\n\
                break;\n\
        }}\n\
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;\n\
        if (lm < 40){{lm = 40;}}\n\
        if (rm < 40){{rm = 40;}}\n\
        if (lm > 100){{lm = 100;}}\n\
        if (rm > 100){{rm = 100;}}\n\
        }}\n\
\n\
        set_motors(lm, rm);\n\
        inputs[4]   = messages;\n\
\n\
        neighbour_dist_log = neighbour_dist;\n\
//straightforward scoring:\n\
/*\n\
        score += neighbour_dist; neighbour_dist = 0;\n\
*/\n\
\n\
\n\
\n\
//For scoring less frequently:\n\
score_tick++;\n\
int score_every = 40;\n\
if (score_tick > score_every){{\n\
        score += neighbour_dist * score_every; score_tick = 0; neighbour_dist = 0;\n}}\n\
\n\
\n\
/*\n\
//for exponential distance reward:\n\
score += neighbour_dist * neighbour_dist;//\n\
*/\n\
\n\
/*\n\
//For scoring exponentially and less frequently:\n\
score_tick++\n\
int score_every = 10\n\
if (score_tick > score_every){{\n\
        score += neighbour_dist * neighbour_dist * score_every; score_tick = 0;}}\n\
*/\n\
\n\
        messages    = 0;\n\
\n\
        last_region = region;\n\
\n\
#ifdef DEBUG\n\
        printf("%lu %d %d\\r\\n", kilo_ticks,region,carrying);\n\
#endif\n\
    }}\n\
\n\
    //===========================================================================\n\
    // Stage only, non kilobot logging\n\
    {{\n\
        usec_t time = world_us_simtime;\n\
        if (time - last_time >= 1e6)\n\
        {{\n\
            last_time += 1e6;\n\
            char buf[1024];\n\
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?\n\
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12i,%12i,%12i \\n", pos->Token(), time/1e6,\n\
                     pos->GetPose().x, pos->GetPose().y, neighbour_dist_log, distance, score); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);\n\
            Evokilo1::log(buf);\n\
        }}\n\
     }}\n\
\n\
}}\n\
\n\
void Evokilo8::setup()\n\
{{\n\
    kilo_message_tx         = (message_tx_t)&Evokilo8::tx_message;\n\
    kilo_message_rx         = (message_rx_t)&Evokilo8::message_rx;\n\
    msg.type    = NORMAL;\n\
    msg.crc     = message_crc(&msg);\n\
    last_region = 0;\n\
    bold = (rand_hard()%255);\n\
    msg.data[0] = bold;\n\
    //  target = rand_hard()%26;\n\
    target = (int)bold/10;\n\
}}\n\
\n\
\n\
\n\
\n\
void Evokilo8::loop()\n\
{{\n\
    int region;\n\
    if (kilo_ticks > last_update + 16)\n\
    {{\n\
        last_update = kilo_ticks;\n\
        updates++;\n\
        region      = get_environment();\n\
        if (region != last_region)\n\
            {{stuck_in_region = 0;\n\
            direction = region - last_region;\n\
            if (direction == -2) {{direction = 1;}}\n\
            if (direction == 2) {{direction = -1;}}\n\
            distance += direction;\n\
            if (target > distance) {{targetdirection = 1;}}\n\
            if (target < distance) {{targetdirection = -1;}}\n\
            if (direction == -targetdirection) {{region=4; turning = 1;}} // if heading in the wrong direction, turn around\n\
                else\n\
                    if (turning == 1) {{lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}}}} // undo hard turn after it has been executed\n\
        else\n\
            {{stuck_in_region +=1;}}\n\
        //if (distance == target) {{target = rand_hard()%26; if (target > distance) {{targetdirection = 1;}} if (target < distance) {{targetdirection = -1;}}}}\n\
        if (distance == target) {{target = (int)bold/10; if (target > distance) {{targetdirection = 1;}} if (target < distance) {{targetdirection = -1;}}}}\n\
\n\
        if (turning == 1){{region = 4;}}\n\
        int d = region;\n\
        region = get_environment();\n\
        switch(d)\n\
        {{\n\
            case(0):\n\
				// Random walk in nest\n\
				//set_motors(70, 60);\n\
				if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(1):\n\
                //set_motors(70, 70);\n\
                if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(2):\n\
				//set_motors(70, 70);\n\
				if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(3):\n\
				//set_motors(70, 70);\n\
				if (updates % 20 != updates)\n\
				{{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;\n\
					updates = updates % 20;}}\n\
                break;\n\
            case(4):\n\
                if (lm > rm) {{lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}}\n\
                else {{lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}}\n\
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)\n\
				//std::cout << "backwards!";\n\
				region      = get_environment();\n\
                break;\n\
            case(5): //an attempt to break out of getting stuck in circles\n\
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;\n\
				region      = get_environment();\n\
                break;\n\
        }}\n\
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;\n\
        if (lm < 40){{lm = 40;}}\n\
        if (rm < 40){{rm = 40;}}\n\
        if (lm > 100){{lm = 100;}}\n\
        if (rm > 100){{rm = 100;}}\n\
\n\
        set_motors(lm, rm);\n\
        if (carrying && (region == NEST) && (last_region == NEST))\n\
        {{\n\
            carrying = 0;\n\
            total_food++;\n\
        }}\n\
        else if (!carrying && (region == FOOD) && (last_region == FOOD))\n\
        {{\n\
            carrying = 1;\n\
            total_pickup++;\n\
        }}\n\
\n\
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;\n\
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;\n\
        inputs[2]   = carrying ? 1.0 : -1.0;\n\
        inputs[4]   = messages;\n\
//        set_color(RGB(1+targetdirection, 2-targetdirection, 0));\n\
        set_color(RGB(bold*3/255, 3-bold*3/255, 3-bold*3/255));\n\
        float a = {0}; // related to how \'stubborn\' the kilobots are in becoming bolder as they get bolder\n\
//also add in a speed of increase and decrease, not just stubbornness! \n\
        float b = {1}; // related to how addictive the kilobots find increasing boldness\n\
        float c = {2}; // related to how \'stubborn\' the kilobots are in becoming shyer as they get bolder\n\
        //increases proportional to current boldness\n\
        float increase = (1/(a*(float)bold+1)) * (float)messages*(float)neighbour_bold/255;\n\
        //float increase = (float)messages*(float)neighbour_bold/255;\n\
        if ((float)neighbour_bold/(float)messages > bold)\n\
            {{\n\
            if (bold + addict*(int)increase > 255) {{bold = 255;}}\n\
            else {{bold = bold + addict*(int)increase; addict += b;}}\n\
//            else {{bold = bold + addict*(int)increase;}}\n\
            }}\n\
\n\
/*\n\
        float increase = (float)messages*(float)neighbour_bold/255;\n\
        if (bold + (int)increase > 255) {{bold = 255;}}\n\
        else {{bold = bold + (int)increase;}}\n\
        if ((int)bold - 1 < 1){{bold = 1;}}\n\
        else {{bold = bold - 1;}} // should probably do this exponentially\n\
*/\n\
        if (messages == 0){{\n\
//            if (19*bold/20 < 1){{bold = 1;}}else\n\
            //slow for high bold, fast for low - attempt to make 2 populations\n\
            if ((bold - 1/(c*(float)bold+1)) < 1){{bold = 1;}} // maybe add a little thermal noise\n\
//            if (bold - 20/(float)bold < 1){{bold = 1;}}\n\
            else {{bold = bold - 1/(c*(float)bold+1); addict = 1;}} // maybe also tick down by 1, so it doesn\'t get stuck \n\
            //{{bold = 49*bold/50; addict = 1;}}\n\
        }}\n\
\n\
        msg.data[0] = bold;\n\
\n\
        inputs[5] = (int)bold;\n\
        neighbour_dist_log = neighbour_dist;\n\
\n\
        messages    = 0;\n\
        msgsum      = 0;\n\
        neighbour_bold = 0;\n\
        neighbour_dist = distance;\n\
        msg.data[1] = neighbour_dist;\n\
\n\
        // remember last region visited\n\
        last_region = region;\n\
        last_output = d;\n\
\n\
#ifdef DEBUG\n\
        printf("%lu %d %d\\r\\n", kilo_ticks,region,carrying);\n\
#endif\n\
    }}\n\
\n\
    //===========================================================================\n\
    // Stage only, non kilobot logging\n\
    {{\n\
        usec_t time = world_us_simtime;\n\
        if (time - last_time >= 1e6)\n\
        {{\n\
            last_time += 1e6;\n\
            char buf[1024];\n\
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?\n\
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i,%12i\\n", pos->Token(), time/1e6,\n\
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection, neighbour_dist_log); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);\n\
            Evokilo8::log(buf);\n\
        }}\n\
     }}\n\
\n\
}}'.format(str(a), str(b), str(c))
            writeFile.write(lines)


if __name__ == '__main__':
  fire.Fire(gen_kiloscript_3)
