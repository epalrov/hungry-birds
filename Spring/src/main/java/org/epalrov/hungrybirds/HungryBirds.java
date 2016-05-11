/*
 * HungryBirds.java - example of concurrent multi-thread application
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import org.epalrov.hungrybirds.HungryBirdsEnv;
import org.epalrov.hungrybirds.HungryBirdsBaby;
import org.epalrov.hungrybirds.HungryBirdsParent;

import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;
 
public class HungryBirds {

    public HungryBirds() {
        /* init bean container */
        ApplicationContext ctx =
            new ClassPathXmlApplicationContext("hungry-birds-beans.xml");

        /* init environment */
        HungryBirdsEnv env = (HungryBirdsEnv) ctx.getBean("hungryBirdsEnv");

        /* start parent and baby birds */
        (new Thread((Runnable) ctx.getBean("hungryBirdsParent", env))).start();
        for (int i = 0; i < env.getBirds(); i++)
            (new Thread((Runnable) ctx.getBean("hungryBirdsBaby", env, i))).start();
    }

    public static void main(String[] args) {
        new HungryBirds();
    }

}

