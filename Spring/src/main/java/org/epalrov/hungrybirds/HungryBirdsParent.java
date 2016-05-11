/*
 * HungryBirdsParent.java - example of parent bird bean
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import org.epalrov.hungrybirds.HungryBirdsEnv;

public class HungryBirdsParent implements Runnable {

    private HungryBirdsEnv env;

    public HungryBirdsParent(HungryBirdsEnv env) {
        this.env = env;
    }

    public HungryBirdsEnv getEnv() {
        return this.env;
    }
    public void setEnv(HungryBirdsEnv env) {
        this.env = env;
    }

    @Override
    public void run() {
        while(true) {
            try {
                env.waitOnFull();
                env.envLock();
                env.setWorms(env.getDish());
                System.out.format(" + parent bird brings" +
                     " %d worms\n\n", env.getWorms());
                env.envUnlock();
                env.signalFull();
            } catch (InterruptedException e) {
                System.out.println(e);
            } catch (Exception e) {
                System.out.println(e);
            }
        }
    }

}

