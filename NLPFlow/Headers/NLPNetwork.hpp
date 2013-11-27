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
#include "NLPTesting.hpp"
#include <boost/ptr_container/ptr_map.hpp>

namespace NLP {
    
    // abstract base class
    class Network : public Testable {
    public:
        Network();
        bool connect(std::string boxOut, std::string portOut, std::string boxIn, std::string portIn);
        bool isRunnable();
        void addComponent(std::string name, Component* component);
        Component& component(std::string name);
        void run();
 
        virtual bool define() = 0;
        
        const std::string str() const;
        
    private:
        struct Wrapper;
        std::shared_ptr<Wrapper> mWrapper;
        
    };
    
    // specific network
    class NLPNetwork : public Network {
    public:
        NLPNetwork();
        bool define();
        static void test();
    };
}

#endif
