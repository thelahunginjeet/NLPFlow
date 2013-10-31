//
//  NLPComponent.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPComponent.hpp"

namespace NLP {
    
# pragma mark - Component methods
    
    Component::Component(std::string name) : mName(name) {}
    
    const std::string Component::name() const {
        return mName;
    }
    
    InputPort& Component::inputPort(std::string name) {
        return mInputs.find(name)->second;
    }
    
    OutputPort& Component::outputPort(std::string name) {
        return mOutputs.find(name)->second;
    }
    
    bool Component::hasOpenPorts() {
        // check for open inputs
        for(auto it = mInputs.begin(); it != mInputs.end(); ++it) {
            if(it->second.isOpen()) {
                return true;
            }
        }
        // check for open outputs
        for(auto it = mOutputs.begin(); it != mOutputs.end(); ++it) {
            if(it->second.isOpen()) {
                return true;
            }
        }
        return false;
    }
    
    void Component::execute() {
        return;
    }
    
}