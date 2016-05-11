/*
 * HungryBirdsBabyFactory.java - example of baby bird factory
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import org.epalrov.hungrybirds.HungryBirdsEnv;
import org.epalrov.hungrybirds.HungryBirdsBaby;

class HungryBirdsBabyFactory {
    public static HungryBirdsBaby newInstance(HungryBirdsEnv env, int bird) {
        return new HungryBirdsBaby(env, bird);
    }
}
