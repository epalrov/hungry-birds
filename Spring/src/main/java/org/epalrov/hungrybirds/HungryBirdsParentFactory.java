/*
 * HungryBirdsParentFactory.java - example of parent bird factory
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

package org.epalrov.hungrybirds;

import org.epalrov.hungrybirds.HungryBirdsEnv;
import org.epalrov.hungrybirds.HungryBirdsParent;

class HungryBirdsParentFactory {
    public static HungryBirdsParent newInstance(HungryBirdsEnv env) {
        return new HungryBirdsParent(env);
    }
}
