#!/usr/bin/env python

"""hungry_birds_class.py - example of concurrent multi-thread application"""

__copyright__ = "Copyright (C) 2015 Andrea Sindoni, Paolo Rovelli"
__author__ = "Andrea Sindoni, Paolo Rovelli"
__email__ = "paolorovelli@yahoo.it"

import threading
import time
import random

NUM_BIRDS = 7
NUM_WORMS = 13


class parent_bird(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    
    def run(self):
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

class baby_bird(threading.Thread):
    def __init__(self, id):
        threading.Thread.__init__(self)
        self.id = id

    def run(self):
        global worms
        while True:
            time.sleep(random.random() * NUM_BIRDS/NUM_WORMS)
            any.acquire()
            with lock:
                worms = worms - 1
                try:
                    if worms :
                        print " - baby bird", self.id, "eats (dish:", worms, "worms)"
                    else :
                        print " - baby bird", self.id, "eats (dish:", worms, "worms)" \
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

    t = parent_bird()
    t.start()
    birds.append(t)

    for i in range(NUM_BIRDS):
        t = baby_bird(i)
        t.start()
        birds.append(t)

    for t in birds:
        t.join()

