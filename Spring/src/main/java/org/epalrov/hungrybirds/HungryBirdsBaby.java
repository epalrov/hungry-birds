/*
 * HungryBirdsBaby.java - example of baby bird bean
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import org.epalrov.hungrybirds.HungryBirdsEnv;

public class HungryBirdsBaby implements Runnable {

    private HungryBirdsEnv env;
    private int bird;

    public HungryBirdsBaby(HungryBirdsEnv env, int bird) {
        this.env = env;
        this.bird = bird;
    }

    public HungryBirdsEnv getEnv() {
        return this.env;
    }
    public void setEnv(HungryBirdsEnv env) {
        this.env = env;
    }

    public int getBird() {
        return this.bird;
    }
    public void setBird(int bird) {
        this.bird = bird;
    }

    @Override
    public void run() {
        while (true) {
            try {
                /* sleep */
                Thread.sleep((int)(Math.random()
                    * (1000*env.getBirds())/env.getDish() + 1));

                /* eat and scream */
                env.waitOnEmpty();
                env.envLock();
                env.setWorms(env.getWorms() - 1);
                if (env.getWorms() != 0) {
                    System.out.format(" - baby bird %d eats" +
                        " (dish: %d worms)\n", bird, env.getWorms());
                    env.envUnlock();
                } else {
                    System.out.format(" - baby bird %d eats" + 
                        " (dish: %d worms)" +
                        " and screams\n\n", bird, env.getWorms());
                    env.envUnlock();
                    env.signalEmpty();
                }
            } catch (InterruptedException e) {
                System.out.println(e);
            } catch (Exception e) {
                System.out.println(e);
            }
        }            
    }

}

