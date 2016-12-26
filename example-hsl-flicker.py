#!/usr/bin/env pypy
import usbbb
import colorsys
import math
import random

bb = usbbb.BB()

h = 0

while True:
    for y in range(0,10):
        for x in range(0,10):
            c = colorsys.hls_to_rgb(h+math.sqrt((x-4.5)**2 + (y-4.5)**2)/10,0.5,1.0)
            if(random.randint(0, 100) == 0):
                c = (0,0,0)
            bb.set_led10(x,y,int(c[0]*255), int(c[1]*255), int(c[2]*255))

    h = (h + .005) % 1.0
    bb.wait_measure()
    bb.transmit(-1)
