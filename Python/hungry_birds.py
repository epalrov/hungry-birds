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
        empty.acquire()
        with lock:
            try:
                worms = NUM_WORMS
                print " + parent bird brings", worms, "worms\n"
                for i in range(NUM_WORMS):
                    any.release()
            except Exception as e:
                    print e.message

def baby_bird(id):
    global worms
    while True:
        time.sleep(random.random() * NUM_BIRDS/NUM_WORMS)
        any.acquire()
        with lock:
            worms = worms - 1
            try:
                if worms:
                    print " - baby bird", id, "eats (dish:", worms, "worms)"
                else:
                    print " - baby bird", id, "eats (dish:", worms, "worms)" \
                        " and screams\n"
                    empty.release()
            except Exception as e:
                    print e.message

if __name__ == "__main__":

    birds = []
    worms = NUM_WORMS
    print "\n + intitial dish:", worms, "worms\n"

    lock = threading.Lock()
    empty = threading.Semaphore(0)
    any = threading.Semaphore(NUM_WORMS)

    t = threading.Thread(target=parent_bird)
    t.start()
    birds.append(t)

    for i in range(NUM_BIRDS):
        t = threading.Thread(target=baby_bird, args=(i,))
        t.start()
        birds.append(t)

    for t in birds:
        t.join()

