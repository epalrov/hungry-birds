/*
 * HungryBirds.java - example of concurrent multi-thread application
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import java.util.concurrent.Semaphore;
 
public class HungryBirds {

    private static final int NUM_BIRDS = 7;
    private static final int NUM_WORMS = 13;

    private int birds;
    private int worms;

    private Semaphore lock;
    private Semaphore empty;
    private Semaphore any;

    public HungryBirds() {
        /* init sync objects */
        lock = new Semaphore(1);
        empty = new Semaphore(0);
        any = new Semaphore(NUM_WORMS);

        /* init shared resources */
        worms = NUM_WORMS;
        System.out.format("\n + intial dish: %d worms" +
            "\n\n", worms);

        /* init and start threads */
        (new Thread(new HungryBirdsParent())).start();
        for (birds = 0; birds < NUM_BIRDS; birds++)
            (new Thread(new HungryBirdsBaby(birds))).start();

    }

    private class HungryBirdsBaby implements Runnable {

        private int bird;

        public HungryBirdsBaby(int bird) {
            this.bird = bird;
        }

        @Override
        public void run() {
            while (true) {
                try {
                    Thread.sleep((int)(Math.random()
                        * (1000*NUM_BIRDS)/NUM_WORMS + 1));
                    any.acquire();
                    lock.acquire();
                    worms--;
                    if (worms != 0) {
                        System.out.format(" - baby bird %d eats" +
                            " (dish: %d worms)\n", bird, worms);
                        lock.release();
                    } else {
                        System.out.format(" - baby bird %d eats" + 
                            " (dish: %d worms)" +
                            " and screams\n\n", bird, worms);
                        lock.release();
                        empty.release();
                    }
                } catch (InterruptedException e) {
                    System.out.println(e);
                } catch (Exception e) {
                    System.out.println(e);
                }
            }            
        }

    }

    private class HungryBirdsParent implements Runnable {

        @Override
        public void run() {
            while(true) {
                try {
                    empty.acquire();
                    lock.acquire();
                    worms = NUM_WORMS;
                    System.out.format(" + parent bird brings" +
                         " %d worms\n\n", worms);
                    lock.release();
                    any.release(worms);
                } catch (InterruptedException e) {
                    System.out.println(e);
                } catch (Exception e) {
                    System.out.println(e);
                }
            }
        }

    }

    public static void main(String[] args) {
        new HungryBirds();
    }

}

