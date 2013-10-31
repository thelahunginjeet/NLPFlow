//
//  NLPComponent.h
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPComponent_hpp
#define NLP_NLPComponent_hpp

#include "NLPPort.hpp"

namespace NLP {
    
    class Component {
    public:
        Component(std::string name = "BLACKBOX");
        const std::string name() const;
        InputPort& inputPort(std::string name);
        OutputPort& outputPort(std::string name);
        bool hasOpenPorts();
        // overload; default does nothing
        virtual void execute();
        
    protected:
        std::string mName;
        std::map<std::string,OutputPort> mOutputs;
        std::map<std::string,InputPort> mInputs;
    };
}

#endif
