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

class HungryBirds(object):
    def __init__(self):

        self._worms = NUM_WORMS
        self._lock = threading.Lock()
        self._empty = threading.Semaphore(0)
        self._any = threading.Semaphore(NUM_WORMS)
        print "\n + intitial dish:", self._worms, "worms\n"

        t = HungryBirdsParent(self)
        t.start()
        b = []
        b.append(t)

        for i in range(NUM_BIRDS):
            t = HungryBirdsBaby(self, i)
            t.start()
            b.append(t)

        for t in b:
            t.join()

class HungryBirdsParent(threading.Thread):
    def __init__(self, hungry):
        self.__hungry = hungry
        threading.Thread.__init__(self)
    
    def run(self):
        while True:
            self.__hungry._empty.acquire()
            with self.__hungry._lock:
                try:
                    self.__hungry._worms = NUM_WORMS
                    print " + parent bird brings", self.__hungry._worms, "worms\n"
                    for i in range(NUM_WORMS):
                        self.__hungry._any.release()
                except Exception as e:
                    print e.message

class HungryBirdsBaby(threading.Thread):
    def __init__(self, hungry, i):
        self.__hungry = hungry
        self.__id = i
        threading.Thread.__init__(self)

    def run(self):
        while True:
            time.sleep(random.random() * NUM_BIRDS/NUM_WORMS)
            self.__hungry._any.acquire()
            with self.__hungry._lock:
                self.__hungry._worms = self.__hungry._worms - 1
                try:
                    if self.__hungry._worms:
                        print " - baby bird", self.__id, "eats (dish:", \
                            self.__hungry._worms, "worms)"
                    else:
                        print " - baby bird", self.__id, "eats (dish:", \
                            self.__hungry._worms, "worms) and screams\n"
                        self.__hungry._empty.release()
                except Exception as e:
                    print e.message

if __name__ == "__main__":
    HungryBirds()

