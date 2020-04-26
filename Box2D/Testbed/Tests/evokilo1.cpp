//----------------------------------------------------------
// Evokilo1 - First experiment in evolved kilobot controller
// (c) Simon Jones 2015
//----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "kilolib.h"
//#include "kilolibc.c"
#include "kilolibc.h"
#include "evokilo1.h"

using namespace Kilolib;

//-------------------------------------------------------------
// Add your file logging pointer here
//-------------------------------------------------------------
// Single class variable for logging file pointer
FILE *Evokilo1::lfp         = NULL;
FILE *Evokilo5::lfp         = NULL;
FILE *Evokilo6::lfp         = NULL;
FILE *Evokilo7::lfp         = NULL;
FILE *Evokilo8::lfp         = NULL;
FILE *Evokilo9::lfp         = NULL;
FILE *Evokilo10::lfp         = NULL;
FILE *Evokilo11::lfp         = NULL;
FILE *Evokilo12::lfp         = NULL;

//-------------------------------------------------------------
uint32_t last_update = 0; //timer for periodic Kilobot instruction

int direction_before=4; //initiate to something that is not 0-3

int random_walk=1; // random walk behavior on/off
int avoid_behavior = 1; //avoidance behavior on/off

int turn_timer = 0;
uint32_t avoid_timer = 0; //avoidance timer

uint32_t random_duration = 0;
uint32_t random_timer = 0;

int behavior_complete=1;

//-------------------------------------------------------------

#define NEST 0
#define FOOD 3


//-------------------------------------------------------------
// This kilobot sits in the middles and counts points so long as the network is connected
void Evokilo1::setup()
{
    kilo_message_tx         = (message_tx_t)&Evokilo1::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo1::message_rx;
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);
    msg.data[0] = 0;
    msg.data[1] = 0;
}


void Evokilo1::loop()
{
    int region;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        if (distance == 0){
        lm = 0;
        rm = 0;
        }
        else {
        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}
        }

        set_motors(lm, rm);
        inputs[4]   = messages;

        neighbour_dist_log = neighbour_dist;

        score += neighbour_dist;

        messages    = 0;
        neighbour_dist = 0;

        last_region = region;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12i,%12i,%12i \n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y, neighbour_dist_log, distance, score); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo1::log(buf);
        }
     }

}
//-------------------------------------------------------------

//-------------------------------------------------------------
void Evokilo5::setup()
// chooses random target, chooses again when this is reached
{
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo5::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo5::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%255);
    //msg.data[0] = 1;
    //uint8_t *b = &bold;
    //memcpy(&b, &bold, sizeof b);
    //b = new uint8_t[bold];
    //memcpy(msg.data[9], &bold, 4);
    msg.data[0] = bold;

        //std::cout << "bold is ";
        //std::cout << (int)bold;
        //std::cout << '\n';

        //std::cout << "bold int is ";
        //std::cout << b;
        //std::cout << '\n';

        //std::cout << "msg is ";
        //std::cout << msgsum;
        //std::cout << msg.data[9];
        //std::cout << '\n';

    target = rand_hard() % 26; // originally at 14
}


void Evokilo5::loop()
{
    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (distance == -1) {distance = 0;} //sometimes pushing can cause kilobots to miss a count
            if (target > distance) {targetdirection = 1;}
            if (target < distance) {targetdirection = -1;}
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        //originally chose from range up to 14
        if (distance == target) {target = rand_hard() % 26; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //direction = region - last_region;
        //std::cout << "direction is ";
        //std::cout << direction;
        //std::cout << '\n';

        //std::cout << "distance is ";
        //std::cout << distance;
        //std::cout << '\n';

        //std::cout << "left motor = ";
        //std::cout << lm;
        //std::cout << '\n';

        //std::cout << "right motor = ";
        //std::cout << rm;
        //std::cout << '\n';

        //std::cout << "neighbours = ";
        //std::cout << messages;
        //std::cout << '\n';


        //std::cout << "boldness = ";
        //std::cout << bold;
        //std::cout << '\n';

/*        //turn depending on where the target region is ---- moved into loop
        if (target > distance) {targetdirection = 1;}
        if (target < distance) {targetdirection = -1;}
        if (direction == -targetdirection) {region=4;} */
        //if (stuck_in_region > 50) {region=5;} // 50 was chosen quite arbitratilly

        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);
        /*
        //// Random walk??
            // Generate an 8-bit random number (between 0 and 2^8 - 1 = 255).
            int random_number = rand_hard();
            // Compute the remainder of random_number when divided by 4.
            // This gives a new random number in the set {0, 1, 2}.
            int random_direction = (random_number % 3);

            if (random_direction != direction_before)
            {
                direction_before=random_direction;

                if (random_direction == 0)
                {
                    spinup_motors();
                    set_motors(kilo_turn_left, 0);
                }
                else if (random_direction == 1)
                {
                    spinup_motors();
                    set_motors(0, kilo_turn_right);
                }
                else if (random_direction == 2)
                {
                    spinup_motors();
                    set_motors(kilo_straight_left, kilo_straight_right);
                }

            }
            */

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest - has to be run to not crash???
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        // inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;
        // Average message
        //float avgmsg = messages > 0 ? msgsum / messages : 0.0;
        //inputs[5]   = avgmsg;

        // Run the neural net - has to be run to not crash???
        //outputs     = nn.nn_update(&inputs[0]);
        /*
        // Motor control
        int d = (outputs[0] >= 0 ? 1 : 0) | (outputs[1] >= 0 ? 2 : 0);
        switch(d)
        {
            case(0):
                set_motors(0,0);
                break;
            case(1):
                if (last_output == 0) spinup_motors();
                set_motors(kilo_turn_left,0);
                set_motors(0,0);
                //set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case(2):
                if (last_output == 0) spinup_motors();
                set_motors(0,kilo_turn_right);
                //set_motors(0,0);
                //set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case(3):
                if (last_output == 0) spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
                set_motors(0,0);
                break;
        }

        // Message output direct from neuron
        float m = outputs[2];
        memcpy(msg.data, &m, 4);
        msg.crc     = message_crc(&msg);


        // visualise the internal state
        set_color(RGB(0, carrying?2:0, 0));
        set_color_msg((outputs[2] + 1.0) / 2.0);


        //set_color_msg(carrying);
        */
        set_color(RGB(1+targetdirection, 2-targetdirection, 0));

        // boldness increases in the presence of other spiders - simple tick up tick down approach
        //if ((int)bold + messages > 255){bold = 255;}
        //else {bold = bold + messages;} // should probably do this exponentially
        // boldness increases proportionatly with those around the kilobot
        float increase = (float)messages*(float)neighbour_bold/255;
        if (bold + (int)increase > 255) {bold = 255;}
        else {bold = bold + (int)increase;}
        //bold = bold + neighbour_bold/50;//messages; //msgsum*0.0005; // should probably do this exponentially
        // and slowly decreases over time
        if ((int)bold - 1 < 1){bold = 1;}
        else {bold = bold - 1;} // should probably do this exponentially


/*
        //Fermi-Dirac like distribution with neighbours' boldness as temperature
        float fbold = 255 - 510*(1/(1+exp((bold-255)/(neighbour_bold+10))));
        //Use this as a 'target temperature' (could heat like a PID?)
        //std::cout << "fbold = ";
        //std::cout << fbold;
        //std::cout << "\n";
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //float change = (fbold-bold)/20;
        //std::cout << "change = ";
        //std::cout << change;
        //std::cout << "\n";
        bold = bold + (fbold-bold)/20+0.5; //0.5 to correct rounding errors 
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //std::cout << (float)bold;
        //std::cout << "\n";
        //bold = (uint8_t)fbold;//255*(1/(1+exp((bold-255)/10)));

        msg.data[0] = bold;
*/
        inputs[5] = (int)bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        inputs[3] = neighbour_bold;
        neighbour_dist_log = neighbour_dist;

        neighbour_dist = distance;
        msg.data[1] = neighbour_dist;
        //std::cout << msg.data[9];
        //std::cout << '\n';
        messages    = 0;
        msgsum      = 0;
        neighbour_bold = 0;
        //min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo5::log(buf);
        }
     }

}
//-------------------------------------------------------------

//-------------------------------------------------------------
void Evokilo6::setup()
{
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo6::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo6::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%100) / 100;

    xtarget = 1 + rand_hard() % 37; // avoiding the large home region by offsetting 3
    ytarget = 1 + rand_hard() % 37; // avoiding the large home region by offsetting 3
}




void Evokilo6::loop()
{
    //for the same, but in a 2D grid
    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            std::cout << "dir = ";
            std::cout << direction;
            std::cout << '\n';
            if (direction == 1 || direction == -4) {xdirection = 1; xdistance += xdirection;} // cycling through - this is still "ascent"
            if (direction == -1 || direction == 4) {xdirection = -1; xdistance += xdirection;} // cycling through - this is "descent"
            if (direction == 2 || direction == -3) {ydirection = 1; ydistance += ydirection;} // cycling through - this is still "ascent"
            if (direction == 3 || direction == -2) {ydirection = -1; ydistance += ydirection;} // cycling through - this is "descent"
            std::cout << "xdir = ";
            std::cout << xdirection;
            std::cout << '\n';
            std::cout << "ydir = ";
            std::cout << ydirection;
            std::cout << '\n';
            if (xtarget > xdistance) {xtargetdirection = 1;}
            if (xtarget < xdistance) {xtargetdirection = -1;}
            if (xtarget == xdistance) {xtargetdirection = 0;}

            if (ytarget > ydistance) {ytargetdirection = 1;}
            if (ytarget < ydistance) {ytargetdirection = -1;}
            if (ytarget == ydistance) {ytargetdirection = 0;}
            if (xdirection == -xtargetdirection || ydirection == -ytargetdirection) {region=5; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        if (xdistance == xtarget && ydistance == ytarget) {xtarget = 1 + rand_hard() % 37; ytarget = 1 + rand_hard() % 37; if (xtarget > xdistance) {xtargetdirection = 1;} if (xtarget < xdistance) {xtargetdirection = -1;} if (ytarget > ydistance) {ytargetdirection = 1;} if (ytarget < ydistance) {ytargetdirection = -1;}}

        if (turning == 1){region = 5;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(5):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(6): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest - has to be run to not crash???
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        //inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;
        set_color(RGB(1+xtargetdirection, 2-xtargetdirection, 1+ytargetdirection));

        // boldness increases in the presence of other spiders
        bold = bold + messages*0.01; // should probably do this exponentially
        if (bold > 1){bold = 1;}

        // and slowly decreases over time
        bold = bold - 0.001; // should probably do this exponentially
        if (bold < 0){bold = 0;}

        inputs[5] = bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        messages    = 0;
        msgsum      = 0;
        //min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y, inputs[4], inputs[5], xdistance, ydistance, xtarget, ytarget, xtargetdirection, ytargetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo6::log(buf);
        }
     }

}
//-------------------------------------------------------------
void Evokilo7::setup()
{
    // moves in/out relatively, not to a destination
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo7::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo7::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%100) / 100;

    target = rand_hard() % 27; // avoiding the large home region by offsetting 3
}



//need to make these private, not global - set in evokilo.h
//int direction;// = 0;
//int lm = 70;
//int rm = 70;
//int nobold = 0;
//float bold; //could be better, random gen?

void Evokilo7::loop()
{
    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        update_target++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (update_target%10 != update_target){targetdirection = 2*(rand_hard()%2)-1; update_target = update_target%10;}//
            targetdirection = 2*(rand_hard()%2)-1; //or do it every time
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}

        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        //inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;

        set_color(RGB(1+targetdirection, 2-targetdirection, 0));

        // boldness increases in the presence of other spiders
        bold = bold + messages*0.01; // should probably do this exponentially
        if (bold > 1){bold = 1;}

        // and slowly decreases over time
        bold = bold - 0.001; // should probably do this exponentially
        if (bold < 0){bold = 0;}

        inputs[5] = bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        messages    = 0;
        msgsum      = 0;
        min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y, inputs[4], inputs[5], distance, target, lm, rm, targetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            //Evokilo7::log(buf);
        }
    }

}
//-------------------------------------------------------------

//-------------------------------------------------------------
// This kilobot sets its target based on boldness level
void Evokilo8::setup()
{
    kilo_message_tx         = (message_tx_t)&Evokilo8::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo8::message_rx;
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);
    last_region = 0;
    bold = (rand_hard()%255);
    msg.data[0] = bold;
    target = (int)bold/10;
}




void Evokilo8::loop()
{
    int region;
    if (kilo_ticks > last_update + 16)
    {
        //neighbour_dist = distance;
        //msg.data[1] = neighbour_dist;
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            {stuck_in_region = 0;
            direction = region - last_region;
            if (direction == -2) {direction = 1;}
            if (direction == 2) {direction = -1;}
            distance += direction;
            if (target > distance) {targetdirection = 1;}
            if (target < distance) {targetdirection = -1;}
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        if (distance == target) {target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}

        if (turning == 1){region = 4;} // maybe make the spin spiral outwards, rather than just continue to loop
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        inputs[2]   = carrying ? 1.0 : -1.0;
        inputs[4]   = messages;
        set_color(RGB(1+targetdirection, 2-targetdirection, 0));

        float a = 5; // related to how 'stubborn' the kilobots are in becoming bolder as they get bolder
        float b = 0.2; // related to how addictive the kilobots find increasing boldness
        float c = 4; // related to how 'stubborn' the kilobots are in becoming shyer as they get bolder
        //increases proportional to current boldness
        float increase = (1/(a*(float)bold+1)) * (float)messages*(float)neighbour_bold/255;
        //float increase = (float)messages*(float)neighbour_bold/255;
        if ((float)neighbour_bold/(float)messages > bold)
            {
            if (bold + addict*(int)increase > 255) {bold = 255;}
            else {bold = bold + addict*(int)increase; addict += b;}
//            else {bold = bold + addict*(int)increase;}
            }

/*
        float increase = (float)messages*(float)neighbour_bold/255;
        if (bold + (int)increase > 255) {bold = 255;}
        else {bold = bold + (int)increase;}
        if ((int)bold - 1 < 1){bold = 1;}
        else {bold = bold - 1;} // should probably do this exponentially
*/
        if (messages == 0){
//            if (19*bold/20 < 1){bold = 1;}else
            //slow for high bold, fast for low - attempt to make 2 populations
            if (bold - 20/(float)bold < 1){bold = 1;} // maybe add a little thermal noise
            else {bold = bold - 1/(c*(float)bold+1); addict = 1;} // maybe also tick down by 1, so it doesn't get stuck 
            //{bold = 49*bold/50; addict = 1;}
        }

        msg.data[0] = bold;

        inputs[5] = (int)bold;
        neighbour_dist_log = neighbour_dist;

        messages    = 0;
        msgsum      = 0;
        neighbour_bold = 0;
        neighbour_dist = distance;
        msg.data[1] = neighbour_dist;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection, neighbour_dist_log); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo8::log(buf);
        }
     }

}
//-------------------------------------------------------------


//-------------------------------------------------------------
// This kilobot sets its taget based on boldness level, and changes its target en-route dynamically
void Evokilo9::setup()
{
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo9::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo9::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%255);
    //msg.data[0] = 1;
    //uint8_t *b = &bold;
    //memcpy(&b, &bold, sizeof b);
    //b = new uint8_t[bold];
    //memcpy(msg.data[9], &bold, 4);
    msg.data[0] = bold;

        //std::cout << "bold is ";
        //std::cout << (int)bold;
        //std::cout << '\n';

        //std::cout << "bold int is ";
        //std::cout << b;
        //std::cout << '\n';

        //std::cout << "msg is ";
        //std::cout << msgsum;
        //std::cout << msg.data[9];
        //std::cout << '\n';

    target = (int)bold/10; // avoiding the large home region by offsetting 3
}




void Evokilo9::loop()
{
    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (target > distance) {targetdirection = 1;}
            if (target < distance) {targetdirection = -1;}
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        // if the target is met, or if there has been a significant change in the siders boldness, it chooses a new target 
        if ((distance == target) || (target > (int)bold/10+2) || (target < (int)bold/10-2)) {target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //if (distance == target) {target = rand_hard() % (1 + (int)bold/10); if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //direction = region - last_region;
        //std::cout << "direction is ";
        //std::cout << direction;
        //std::cout << '\n';

        //std::cout << "distance is ";
        //std::cout << distance;
        //std::cout << '\n';

        //std::cout << "left motor = ";
        //std::cout << lm;
        //std::cout << '\n';

        //std::cout << "right motor = ";
        //std::cout << rm;
        //std::cout << '\n';

        //std::cout << "neighbours = ";
        //std::cout << messages;
        //std::cout << '\n';


        //std::cout << "boldness = ";
        //std::cout << bold;
        //std::cout << '\n';

/*        //turn depending on where the target region is ---- moved into loop
        if (target > distance) {targetdirection = 1;}
        if (target < distance) {targetdirection = -1;}
        if (direction == -targetdirection) {region=4;} */
        //if (stuck_in_region > 50) {region=5;} // 50 was chosen quite arbitratilly
        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
                //region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				//region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);
        /*
        //// Random walk??
            // Generate an 8-bit random number (between 0 and 2^8 - 1 = 255).
            int random_number = rand_hard();
            // Compute the remainder of random_number when divided by 4.
            // This gives a new random number in the set {0, 1, 2}.
            int random_direction = (random_number % 3);

            if (random_direction != direction_before)
            {
                direction_before=random_direction;

                if (random_direction == 0)
                {
                    spinup_motors();
                    set_motors(kilo_turn_left, 0);
                }
                else if (random_direction == 1)
                {
                    spinup_motors();
                    set_motors(0, kilo_turn_right);
                }
                else if (random_direction == 2)
                {
                    spinup_motors();
                    set_motors(kilo_straight_left, kilo_straight_right);
                }

            }
            */

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest - has to be run to not crash???
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        // inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;
        // Average message
        //float avgmsg = messages > 0 ? msgsum / messages : 0.0;
        //inputs[5]   = avgmsg;

        // Run the neural net - has to be run to not crash???
        //outputs     = nn.nn_update(&inputs[0]);
        /*
        // Motor control
        int d = (outputs[0] >= 0 ? 1 : 0) | (outputs[1] >= 0 ? 2 : 0);
        switch(d)
        {
            case(0):
                set_motors(0,0);
                break;
            case(1):
                if (last_output == 0) spinup_motors();
                set_motors(kilo_turn_left,0);
                set_motors(0,0);
                //set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case(2):
                if (last_output == 0) spinup_motors();
                set_motors(0,kilo_turn_right);
                //set_motors(0,0);
                //set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case(3):
                if (last_output == 0) spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
                set_motors(0,0);
                break;
        }

        // Message output direct from neuron
        float m = outputs[2];
        memcpy(msg.data, &m, 4);
        msg.crc     = message_crc(&msg);


        // visualise the internal state
        set_color(RGB(0, carrying?2:0, 0));
        set_color_msg((outputs[2] + 1.0) / 2.0);


        //set_color_msg(carrying);
        */
//        set_color(RGB(1+targetdirection, 2-targetdirection, 0));
        set_color(RGB(bold*3/255, 3-bold*3/255, 3-bold*3/255));

        // boldness increases in the presence of other spiders - simple tick up tick down approach
//        if ((int)bold + messages > 255){bold = 255;}
//        else {bold = bold + messages;} // should probably do this exponentially

        float increase = (float)messages*(float)neighbour_bold/255;
        if (bold + (int)increase > 255) {bold = 255;}
        else {bold = bold + (int)increase;}

        //bold = bold + neighbour_bold/50;//messages; //msgsum*0.0005; // should probably do this exponentially
        // and slowly decreases over time
//        if ((int)bold - 1 < 1){bold = 1;}
//        else {bold = bold - 1;} // should probably do this exponentially

        if (messages == 0){
//            if (19*bold/20 < 1){bold = 1;}else 
            {bold = 39*bold/40;}
        }
        bold = 0;
/*
        //Fermi-Dirac like distribution with neighbours' boldness as temperature
        float fbold = 255 - 510*(1/(1+exp((bold-255)/(neighbour_bold+10))));
        //Use this as a 'target temperature' (could heat like a PID?)
        //std::cout << "fbold = ";
        //std::cout << fbold;
        //std::cout << "\n";
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //float change = (fbold-bold)/20;
        //std::cout << "change = ";
        //std::cout << change;
        //std::cout << "\n";
        bold = bold + (fbold-bold)/20+0.5; //0.5 to correct rounding errors 
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //std::cout << (float)bold;
        //std::cout << "\n";
        //bold = (uint8_t)fbold;//255*(1/(1+exp((bold-255)/10)));

        msg.data[0] = bold;
*/
        inputs[5] = (int)bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        inputs[3] = neighbour_bold;
        //std::cout << msg.data[9];
        //std::cout << '\n';
        messages    = 0;
        msgsum      = 0;
        neighbour_bold = 0;
        //min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo9::log(buf);
        }
     }

}
//-------------------------------------------------------------

//-------------------------------------------------------------
// This kilobot sets its taget based on boldness level, and changes its target en-route dynamically. It only increases boldness if its neighbours are bolder than it.
// It can also have addictive boldness i.e. does it twice in a row -> second is larger
void Evokilo10::setup()
{
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo10::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo10::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%254)+1;
    //msg.data[0] = 1;
    //uint8_t *b = &bold;
    //memcpy(&b, &bold, sizeof b);
    //b = new uint8_t[bold];
    //memcpy(msg.data[9], &bold, 4);
    msg.data[0] = bold;

        //std::cout << "bold is ";
        //std::cout << (int)bold;
        //std::cout << '\n';

        //std::cout << "bold int is ";
        //std::cout << b;
        //std::cout << '\n';

        //std::cout << "msg is ";
        //std::cout << msgsum;
        //std::cout << msg.data[9];
        //std::cout << '\n';

    target = (int)bold/10; // avoiding the large home region by offsetting 3
}




void Evokilo10::loop()
{
    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (target > distance) {targetdirection = 1;}
            if (target < distance) {targetdirection = -1;}
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        // if the target is met, or if there has been a significant change in the siders boldness, it chooses a new target 
        if ((distance == target) || (target > (int)bold/10+2) || (target < (int)bold/10-2)) {target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //if (distance == target) {target = rand_hard() % (1 + (int)bold/10); if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //direction = region - last_region;
        //std::cout << "direction is ";
        //std::cout << direction;
        //std::cout << '\n';

        //std::cout << "distance is ";
        //std::cout << distance;
        //std::cout << '\n';

        //std::cout << "left motor = ";
        //std::cout << lm;
        //std::cout << '\n';

        //std::cout << "right motor = ";
        //std::cout << rm;
        //std::cout << '\n';

        //std::cout << "neighbours = ";
        //std::cout << messages;
        //std::cout << '\n';


        //std::cout << "boldness = ";
        //std::cout << bold;
        //std::cout << '\n';

/*        //turn depending on where the target region is ---- moved into loop
        if (target > distance) {targetdirection = 1;}
        if (target < distance) {targetdirection = -1;}
        if (direction == -targetdirection) {region=4;} */
        //if (stuck_in_region > 50) {region=5;} // 50 was chosen quite arbitratilly
        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        //std::cout << "lmb = ";std::cout << lm;std::cout << "rmb = ";std::cout << rm;
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);
        /*
        //// Random walk??
            // Generate an 8-bit random number (between 0 and 2^8 - 1 = 255).
            int random_number = rand_hard();
            // Compute the remainder of random_number when divided by 4.
            // This gives a new random number in the set {0, 1, 2}.
            int random_direction = (random_number % 3);

            if (random_direction != direction_before)
            {
                direction_before=random_direction;

                if (random_direction == 0)
                {
                    spinup_motors();
                    set_motors(kilo_turn_left, 0);
                }
                else if (random_direction == 1)
                {
                    spinup_motors();
                    set_motors(0, kilo_turn_right);
                }
                else if (random_direction == 2)
                {
                    spinup_motors();
                    set_motors(kilo_straight_left, kilo_straight_right);
                }

            }
            */

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest - has to be run to not crash???
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        // inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;
        // Average message
        //float avgmsg = messages > 0 ? msgsum / messages : 0.0;
        //inputs[5]   = avgmsg;

        // Run the neural net - has to be run to not crash???
        //outputs     = nn.nn_update(&inputs[0]);
        /*
        // Motor control
        int d = (outputs[0] >= 0 ? 1 : 0) | (outputs[1] >= 0 ? 2 : 0);
        switch(d)
        {
            case(0):
                set_motors(0,0);
                break;
            case(1):
                if (last_output == 0) spinup_motors();
                set_motors(kilo_turn_left,0);
                set_motors(0,0);
                //set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case(2):
                if (last_output == 0) spinup_motors();
                set_motors(0,kilo_turn_right);
                //set_motors(0,0);
                //set_motors(kilo_straight_left, kilo_straight_right);
                break;
            case(3):
                if (last_output == 0) spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
                set_motors(0,0);
                break;
        }

        // Message output direct from neuron
        float m = outputs[2];
        memcpy(msg.data, &m, 4);
        msg.crc     = message_crc(&msg);


        // visualise the internal state
        set_color(RGB(0, carrying?2:0, 0));
        set_color_msg((outputs[2] + 1.0) / 2.0);


        //set_color_msg(carrying);
        */
//        set_color(RGB(1+targetdirection, 2-targetdirection, 0));
        set_color(RGB(bold*3/255, 3-bold*3/255, 3-bold*3/255));

        //increases proportional to current boldness
        float increase = (10/((float)bold+1)) * (float)messages*(float)neighbour_bold/255;
        //float increase = (float)messages*(float)neighbour_bold/255;
        if ((float)neighbour_bold/(float)messages > bold)
            {
            if (bold + addict*(int)increase > 255) {bold = 255;}
            else {bold = bold + addict*(int)increase; addict = 1;}
//            else {bold = bold + addict*(int)increase;}
            }

        //bold = bold + neighbour_bold/50;//messages; //msgsum*0.0005; // should probably do this exponentially
        // and slowly decreases over time
//        if ((int)bold - 1 < 1){bold = 1;}
//        else {bold = bold - 1;} // should probably do this exponentially

        if (messages == 0){
//            if (19*bold/20 < 1){bold = 1;}else
            //slow for high bold, fast for low - attempt to make 2 populations
            if ((float)bold - 1/(5*(float)bold) < 1){bold = 1;}
            else {bold = (float)bold - 1/(5*(float)bold); addict = 1;} // maybe also tick down by 1, so it doesn't get stuck 
            //{bold = 49*bold/50; addict = 1;}
        }

/*
        //Fermi-Dirac like distribution with neighbours' boldness as temperature
        float fbold = 255 - 510*(1/(1+exp((bold-255)/(neighbour_bold+10))));
        //Use this as a 'target temperature' (could heat like a PID?)
        //std::cout << "fbold = ";
        //std::cout << fbold;
        //std::cout << "\n";
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //float change = (fbold-bold)/20;
        //std::cout << "change = ";
        //std::cout << change;
        //std::cout << "\n";
        bold = bold + (fbold-bold)/20+0.5; //0.5 to correct rounding errors 
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //std::cout << (float)bold;
        //std::cout << "\n";
        //bold = (uint8_t)fbold;//255*(1/(1+exp((bold-255)/10)));

        msg.data[0] = bold;
*/
        inputs[5] = (int)bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        inputs[3] = neighbour_bold;
        //std::cout << msg.data[9];
        //std::cout << '\n';
        messages    = 0;
        msgsum      = 0;
        neighbour_bold = 0;
        //min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo10::log(buf);
        }
     }

}
//-------------------------------------------------------------


//-------------------------------------------------------------
// This kilobot sets its target based on boldness level, and changes its target when it completes its journey. It only increases boldness if its neighbours are bolder than it.
// It can also have addictive boldness i.e. does it twice in a row -> second is larger
void Evokilo11::setup()
{
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo11::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo11::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%254)+1;
    //msg.data[0] = 1;
    //uint8_t *b = &bold;
    //memcpy(&b, &bold, sizeof b);
    //b = new uint8_t[bold];
    //memcpy(msg.data[9], &bold, 4);
    msg.data[0] = bold;

        //std::cout << "bold is ";
        //std::cout << (int)bold;
        //std::cout << '\n';

        //std::cout << "bold int is ";
        //std::cout << b;
        //std::cout << '\n';

        //std::cout << "msg is ";
        //std::cout << msgsum;
        //std::cout << msg.data[9];
        //std::cout << '\n';

    //target = rand_hard() % 26;
    target = (int)bold/10; // avoiding the large home region by offsetting 3
}




void Evokilo11::loop()
{
    // turn off half the robots at 1500 seconds
    usec_t time = world_us_simtime;
    if (((float)kilo_uid > 32) && (time > 1500e6))
    {
    msg.data[0] = 0;
    bold = 0;
    target = 999999;
    };// & 0xff;
    if (((float)kilo_uid > 16) && (time > 3000e6))
    {
    msg.data[0] = 0;
    bold = 0;
    target = 999999;
    };// & 0xff;

    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (target > distance) {targetdirection = 1;}
            if (target < distance) {targetdirection = -1;}
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        // if the target is met, it chooses a new target 
        if (distance == target)     //{target = rand_hard() % 26; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
                                    {target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //if ((distance == target) || (target > (int)bold/10+2) || (target < (int)bold/10-2)) {target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest - has to be run to not crash???
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        // inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;

//        set_color(RGB(1+targetdirection, 2-targetdirection, 0));
        set_color(RGB(bold*3/255, 3-bold*3/255, 3-bold*3/255));

        //increases proportional to current boldness
        float increase = (10/((float)bold+1)) * (float)messages*(float)neighbour_bold/255;
        //float increase = (float)messages*(float)neighbour_bold/255;
        if ((float)neighbour_bold/(float)messages > bold)
            {
            if (bold + addict*(int)increase > 255) {bold = 255;}
            else {bold = bold + addict*(int)increase; addict += 1;}
//            else {bold = bold + addict*(int)increase;}
            }

        //bold = bold + neighbour_bold/50;//messages; //msgsum*0.0005; // should probably do this exponentially
        // and slowly decreases over time
//        if ((int)bold - 1 < 1){bold = 1;}
//        else {bold = bold - 1;} // should probably do this exponentially

        if (messages == 0){
//            if (19*bold/20 < 1){bold = 1;}else
            //slow for high bold, fast for low - attempt to make 2 populations
            if (bold - 1/(5*(float)bold) < 1){bold = 1;}
            else {bold = bold - 1/(5*(float)bold); addict = 1;} // maybe also tick down by 1, so it doesn't get stuck 
            //{bold = 49*bold/50; addict = 1;}
        }

/*
        //Fermi-Dirac like distribution with neighbours' boldness as temperature
        float fbold = 255 - 510*(1/(1+exp((bold-255)/(neighbour_bold+10))));
        //Use this as a 'target temperature' (could heat like a PID?)
        //std::cout << "fbold = ";
        //std::cout << fbold;
        //std::cout << "\n";
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //float change = (fbold-bold)/20;
        //std::cout << "change = ";
        //std::cout << change;
        //std::cout << "\n";
        bold = bold + (fbold-bold)/20+0.5; //0.5 to correct rounding errors 
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //std::cout << (float)bold;
        //std::cout << "\n";
        //bold = (uint8_t)fbold;//255*(1/(1+exp((bold-255)/10)));
*/

        msg.data[0] = (int)bold; // is this really working? - it seems to be

        inputs[5] = (int)bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        inputs[3] = neighbour_bold;
        //std::cout << msg.data[9];
        //std::cout << '\n';
        messages    = 0;
        msgsum      = 0;
        neighbour_bold = 0;
        //min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo11::log(buf);
        }
     }

}

//-------------------------------------------------------------
// This kilobot sets its target randomly, and changes its target when it completes its journey.
void Evokilo12::setup()
{
    // Set the callbacks
    kilo_message_tx         = (message_tx_t)&Evokilo12::tx_message;
    kilo_message_rx         = (message_rx_t)&Evokilo12::message_rx;

    // Construct a valid message
    msg.type    = NORMAL;
    msg.crc     = message_crc(&msg);

    last_region = 0;
    bold = (rand_hard()%254)+1;
    //msg.data[0] = 1;
    //uint8_t *b = &bold;
    //memcpy(&b, &bold, sizeof b);
    //b = new uint8_t[bold];
    //memcpy(msg.data[9], &bold, 4);
    msg.data[0] = bold;

        //std::cout << "bold is ";
        //std::cout << (int)bold;
        //std::cout << '\n';

        //std::cout << "bold int is ";
        //std::cout << b;
        //std::cout << '\n';

        //std::cout << "msg is ";
        //std::cout << msgsum;
        //std::cout << msg.data[9];
        //std::cout << '\n';

    //target = rand_hard() % 26;
    target = (int)bold/10; // avoiding the large home region by offsetting 3
}




void Evokilo12::loop()
{
    // turn off half the robots at 1500 seconds
    usec_t time = world_us_simtime;
    if (((float)kilo_uid > 32) && (time > 1500e6))
    {
    msg.data[0] = 0;
    bold = 0;
    target = 999999;
    };// & 0xff;
    if (((float)kilo_uid > 16) && (time > 3000e6))
    {
    msg.data[0] = 0;
    bold = 0;
    target = 999999;
    };// & 0xff;

    // Always send a message

    //set boldness
//	if (nobold == 0){bold = (rand_hard()%100) / 100; nobold = 1;}
    int region;
    //int direction;
    if (kilo_ticks > last_update + 16)
    {
        last_update = kilo_ticks;
        updates++;
        region      = get_environment();
        if (region != last_region)
            //direction is simply towards or away from centre
            {stuck_in_region = 0;
            direction = region - last_region; // make a version where the direction is chosen, rather that a target distance
            if (direction == -2) {direction = 1;} // cycling through - this is still "ascent"
            if (direction == 2) {direction = -1;} // cycling through - this is "descent"
            distance += direction;
            if (target > distance) {targetdirection = 1;}
            if (target < distance) {targetdirection = -1;}
            if (direction == -targetdirection) {region=4; turning = 1;} // if heading in the wrong direction, turn around
                else
                    if (turning == 1) {lm = 60 + rand_hard()%10; rm = 60 + rand_hard()%10; turning = 0;}} // undo hard turn after it has been executed
        else
            {stuck_in_region +=1;}
        // if the target is met, it chooses a new target 
        if (distance == target)     {target = rand_hard() % 26; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
                                    //{target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        //if ((distance == target) || (target > (int)bold/10+2) || (target < (int)bold/10-2)) {target = (int)bold/10; if (target > distance) {targetdirection = 1;} if (target < distance) {targetdirection = -1;}}
        if (turning == 1){region = 4;}
        int d = region;
        region = get_environment();
        switch(d)
        {
            case(0):
				// Random walk in nest
				//set_motors(70, 60);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(1):
                //set_motors(70, 70);
                if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(2):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(3):
				//set_motors(70, 70);
				if (updates % 20 != updates)
				{lm = (lm+rm)/2 - 10 + rand_hard()%20; rm = (lm+rm)/2 - 10 + rand_hard()%20;
					updates = updates % 20;}
                break;
            case(4):
                if (lm > rm) {lm = 90 - rand_hard()%5; rm = 50 + rand_hard()%5;}
                else {lm = 50 + rand_hard()%5; rm = 90 - rand_hard()%5;}
				//lm = 50*(rand_hard()%2) + 30; rm = 110-lm; //randomly turn on the spot with motor speed 80:30 (random is to avoid circling)
				//std::cout << "backwards!";
				region      = get_environment();
                break;
            case(5): //an attempt to break out of getting stuck in circles
				lm = 70 + rand_hard()%10; rm = 70 + rand_hard()%10;
				region      = get_environment();
                break;
        }
        if (lm < 40){lm = 40;}
        if (rm < 40){rm = 40;}
        if (lm > 100){lm = 100;}
        if (rm > 100){rm = 100;}

        set_motors(lm, rm);

        // Little state machine for food transport: always collect food if in food area
        // and deposit it if in nest area
        if (carrying && (region == NEST) && (last_region == NEST))
        {
            carrying = 0;
            total_food++;
        }
        else if (!carrying && (region == FOOD) && (last_region == FOOD))
        {
            carrying = 1;
            total_pickup++;
        }

        // Nest - has to be run to not crash???
        inputs[0]   = (region == NEST) ? 1.0 : -1.0;
        // Food
        inputs[1]   = (region == FOOD) ? 1.0 : -1.0;
        // carrying food
        inputs[2]   = carrying ? 1.0 : -1.0;

        // Distance to nearest neighbours
        // inputs[3]   = (float)min_dist / 100.0;
        // Number of neighbours
        inputs[4]   = messages;

//        set_color(RGB(1+targetdirection, 2-targetdirection, 0));
        set_color(RGB(bold*3/255, 3-bold*3/255, 3-bold*3/255));

        //increases proportional to current boldness
        float increase = (10/((float)bold+1)) * (float)messages*(float)neighbour_bold/255;
        //float increase = (float)messages*(float)neighbour_bold/255;
        if ((float)neighbour_bold/(float)messages > bold)
            {
            if (bold + addict*(int)increase > 255) {bold = 255;}
            else {bold = bold + addict*(int)increase; addict += 1;}
//            else {bold = bold + addict*(int)increase;}
            }

        //bold = bold + neighbour_bold/50;//messages; //msgsum*0.0005; // should probably do this exponentially
        // and slowly decreases over time
//        if ((int)bold - 1 < 1){bold = 1;}
//        else {bold = bold - 1;} // should probably do this exponentially

        if (messages == 0){
//            if (19*bold/20 < 1){bold = 1;}else
            //slow for high bold, fast for low - attempt to make 2 populations
            if (bold - 1/(5*(float)bold) < 1){bold = 1;}
            else {bold = bold - 1/(5*(float)bold); addict = 1;} // maybe also tick down by 1, so it doesn't get stuck 
            //{bold = 49*bold/50; addict = 1;}
        }

/*
        //Fermi-Dirac like distribution with neighbours' boldness as temperature
        float fbold = 255 - 510*(1/(1+exp((bold-255)/(neighbour_bold+10))));
        //Use this as a 'target temperature' (could heat like a PID?)
        //std::cout << "fbold = ";
        //std::cout << fbold;
        //std::cout << "\n";
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //float change = (fbold-bold)/20;
        //std::cout << "change = ";
        //std::cout << change;
        //std::cout << "\n";
        bold = bold + (fbold-bold)/20+0.5; //0.5 to correct rounding errors 
        //std::cout << "bold = ";
        //std::cout << (int)bold;
        //std::cout << "\n";
        //std::cout << (float)bold;
        //std::cout << "\n";
        //bold = (uint8_t)fbold;//255*(1/(1+exp((bold-255)/10)));
*/

        msg.data[0] = (int)bold; // is this really working?

        inputs[5] = (int)bold;

        //inputs[6] = distance; // THIS IS CAUSING THE PROBLEM!

        //inputs[7] = target;

        // Clear the message variables
        inputs[3] = neighbour_bold;
        //std::cout << msg.data[9];
        //std::cout << '\n';
        messages    = 0;
        msgsum      = 0;
        neighbour_bold = 0;
        //min_dist    = 150;

        // remember last region visited
        last_region = region;
        last_output = d;

#ifdef DEBUG
        printf("%lu %d %d\r\n", kilo_ticks,region,carrying);
#endif
    }

    //===========================================================================
    // Stage only, non kilobot logging
    {
        usec_t time = world_us_simtime;
        if (time - last_time >= 1e6)
        {
            last_time += 1e6;
            char buf[1024];
            // this gets weird toward the end of large logs... consider cutting out unneeded bits?
            snprintf(buf, 1024, "%12s,%12f,%12f,%12f,%12f,%12f,%12f,%12i,%12i,%12i,%12i,%12i\n", pos->Token(), time/1e6,
                     pos->GetPose().x, pos->GetPose().y,inputs[3],inputs[4], inputs[5], distance, target, lm, rm, targetdirection); //inputs[6], inputs[7]);//, outputs[0],outputs[1], outputs[2]);
            Evokilo12::log(buf);
        }
     }

}



