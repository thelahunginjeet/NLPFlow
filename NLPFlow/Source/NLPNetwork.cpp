//
//  NLPNetwork.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPNetwork.hpp"

namespace NLP {
    
# pragma mark - Network methods
    
    Network::Network() { }
    
    bool Network::connect(std::string boxOut, std::string outName, std::string boxIn, std::string inName)
    {
        auto it1 = mBoxes.find(boxOut);
        auto it2 = mBoxes.find(boxIn);
        (it1->second)->outputPort(outName).connect((it2->second)->inputPort(inName));
        return true;
    }
    
    bool Network::isRunnable() {
        for(auto it = mBoxes.begin(); it != mBoxes.end(); ++it) {
            if((it->second)->hasOpenPorts()) {
                return true;
            }
        }
        return false;
    }
    
    void Network::run() {
        // isRunnable indicates we are capable of running
        while(isRunnable()) {
            // iterate over all the components and try execution
            for(auto it = mBoxes.begin(); it != mBoxes.end(); ++it) {
                (it->second)->execute();
            }
        }
    }
    
}