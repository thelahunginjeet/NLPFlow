//
//  main.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/22/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLP.hpp"
#include <iostream>


int testnetwork() {
    // create the network
    NLP::NLPNetwork network;
    // setup
    bool isOK = network.define();
    // run if the network is hooked up correctly
    if(isOK) {
        network.run();
    } else {
        std::cout << "Network is not properly wired up." << std::endl;
    }
    return 0;
}

int main(int argc, const char * argv[]) {
    testnetwork();
    return 0;
}


