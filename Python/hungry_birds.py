#!/usr/bin/env python

"""hungry_birds.py - example of concurrent multi-thread application"""

__copyright__ = "Copyright (C) 2015 Andrea Sindoni, Paolo Rovelli"
__author__ = "Andrea Sindoni, Paolo Rovelli"
__email__ = "paolorovelli@yahoo.it"

import threading
import time
import random

NUM_BIRDS = 7
NUM_WORMS = 13

def parent_bird():
    global worms
    while True:
        empty_event.wait()
        with lock:
            try:
                worms = NUM_WORMS
                print " + Parent brings ", worms, " worms\n"
                empty_event.clear()
                any_event.set()
            except:
                print " x Exception!\n"

def baby_bird(id):
    global worms
    while True:
        time.sleep(random.random() * NUM_BIRDS/NUM_WORMS)
        any_event.wait()
        with lock:
            worms = worms - 1
            try:
                if worms :
                    print " - Bird ", id, " eats (dish: ", worms, " worms)"
                else :
                    print " - Bird ", id, " eats (dish: ", worms, " worms)" \
                        " and screams\n"
                    any_event.clear()
                    empty_event.set()
            except:
                print " x Exception!\n"

if __name__ == "__main__":

    birds = []
    worms = NUM_WORMS
    lock = threading.Lock()
    any_event = threading.Event()
    empty_event = threading.Event()

    print "\n + intitial dish: ", worms, " worms\n"
    any_event.set()

    t = threading.Thread(target=parent_bird)
    t.start()
    birds.append(t)

    for id in range(NUM_BIRDS):
        t = threading.Thread(target=baby_bird, args=(id,))
        t.start()
        birds.append(t)

    for t in birds:
        t.join()

