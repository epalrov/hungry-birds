/*
 * HungryBirdsEnv.java - example of hungry birds environment
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import java.util.concurrent.Semaphore;
 
public class HungryBirdsEnv {

    private int dish;
    private int worms;
    private int birds;

    private Semaphore lock;
    private Semaphore empty;
    private Semaphore any;

    public HungryBirdsEnv(int worms, int birds) {
        /* init sync objects */
        this.lock = new Semaphore(1);
        this.empty = new Semaphore(0);
        this.any = new Semaphore(worms);

        /* init shared resources */
        this.dish = worms;
        this.worms = worms;
        this.birds = birds;

        /* print init status */
        System.out.format("\n + intial dish: %d worms" +
            "\n\n", this.worms);
    }

    public int getDish() {
        return this.dish;
    }
    public void setDish(int dish) {
        this.dish = dish;
    }

    public int getWorms() {
        return this.worms;
    }
    public void setWorms(int worms) {
        this.worms = worms;
    }

    public int getBirds() {
        return this.birds;
    }
    public void setBirds(int birds) {
        this.birds = birds;
    }

    public void envLock() throws InterruptedException {
        this.lock.acquire();
    }
    public void envUnlock() {
        this.lock.release();
    }

    public void waitOnFull() throws InterruptedException {
        this.empty.acquire();
    }
    public void signalFull() {
        this.any.release(this.dish);
    }

    public void waitOnEmpty() throws InterruptedException {
        this.any.acquire();
    }
    public void signalEmpty() {
        this.empty.release();
    }

}

