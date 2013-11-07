//
//  NLPNetwork.h
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPNetwork_hpp
#define NLP_NLPNetwork_npp

#include "NLPComponent.hpp"

namespace NLP {
    
    // abstract base class
    class Network {
    public:
        Network();
        bool connect(std::string boxOut, std::string portOut, std::string boxIn, std::string portIn);
        bool isRunnable();
        void run();
 
        virtual bool define() = 0;
        
    protected:
        boost::ptr_map<std::string, Component> mBoxes;
    };
    
    // specific network
    class NLPNetwork : public Network {
    public:
        NLPNetwork();
        bool define();
    };
}

#endif
