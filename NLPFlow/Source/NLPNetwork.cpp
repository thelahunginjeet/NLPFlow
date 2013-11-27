//
//  NLPNetwork.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPNetwork.hpp"

namespace NLP {
    
# pragma mark - Wrapper for Network
    
    struct Network::Wrapper {
        Wrapper() {};
        boost::ptr_map<std::string, Component> mBoxes;
    };

    
# pragma mark - Network methods
    
    Network::Network() : mWrapper(new Wrapper()) { }
    
    
    // This should check everyting:
    // - do the boxes exist?
    // - do the boxes contained the requested ports?
    // - does the output-input type match?
    bool Network::connect(std::string boxOut, std::string outName, std::string boxIn, std::string inName)
    {
        auto it1 = (mWrapper->mBoxes).find(boxOut);
        if (it1 == (mWrapper->mBoxes).end()) {
            std::cout << boxOut << " not found." << std::endl;
            return false;
        }
        auto it2 = (mWrapper->mBoxes).find(boxIn);
        if (it2 == (mWrapper->mBoxes).end()) {
            std::cout << boxIn << " not found." << std::endl;
            return false;
        }
        if ((it1->second)->hasOutput(outName) && (it2->second)->hasInput(inName)) {
            return (it1->second)->outputPort(outName).connect((it2->second)->inputPort(inName));
        } else {
            std::cout << "Port name problem." << std::endl;
        }
        return false;
    }
    
    bool Network::isRunnable() {
        for(auto it = (mWrapper->mBoxes).begin(); it != (mWrapper->mBoxes).end(); ++it) {
            if((it->second)->hasOpenPorts()) {
                return true;
            }
        }
        return false;
    }
    
    void Network::addComponent(std::string name, Component* component) {
        (mWrapper->mBoxes).insert(name,component);
    }
    
    Component& Network::component(std::string name) {
        return (mWrapper->mBoxes).at(name);
    }
    
    void Network::run() {
        std::cout << "<---FLOW START--->" << std::endl;
        // this runs the network until there is nothing to do
        while(isRunnable()) {
            // a lot of these statements are just for debugging
            std::cout << "In run loop . . ." << std::endl;
            // iterate over all the components and to run them
            for(auto it = (mWrapper->mBoxes).begin(); it != (mWrapper->mBoxes).end(); ++it) {
                std::cout << (it->second)->str() << std::endl;
                if((it->second)->hasOpenPorts()) {
                    (it->second)->execute();
                }
            }
        }
        std:: cout << "<---FLOW END--->" << std::endl;
    }
    
    const std::string Network::str() const {
        std::stringstream s;
        s << "<Network: ";
        for(auto it = (mWrapper->mBoxes).begin(); it != (mWrapper->mBoxes).end(); ++it) {
            s << (it->second)->str() << " ";
        }
        s << ">";
        return s.str();
    }
    
# pragma mark - NLPNetwork methods
    
    NLPNetwork::NLPNetwork() : Network() {}
    
    bool NLPNetwork::define() {
        bool isWiredUp = true;
        // boxes
        std::string rKey = "READER";
        addComponent(rKey,new TextReader("TextReader"));
        std::string tKey = "TOKENIZER";
        addComponent(tKey,new Tokenizer("Tokenizer"));
        std::string sKey = "SELECTOR";
        addComponent(sKey, new StringSizeSelector("Selector"));
        std::string cKey = "COUNTER";
        addComponent(cKey,new StringCounter("Counter"));
        std::string iKey = "INTWRITER";
        addComponent(iKey,new PacketWriter("IntWriter"));
        std::string wKey = "TXTWRITER";
        addComponent(wKey,new PacketWriter("TextWriter"));
        // connectivity
        isWiredUp = isWiredUp && connect(rKey,"TXTOUT",tKey,"TXTIN");
        isWiredUp = isWiredUp && connect(tKey,"TXTOUT",sKey,"IN");
        isWiredUp = isWiredUp && connect(sKey,"OUT_T",wKey,"PACKIN");
        isWiredUp = isWiredUp && connect(sKey,"OUT_F",cKey,"TXTIN");
        isWiredUp = isWiredUp && connect(cKey,"INTOUT",iKey,"PACKIN");
        // parameters
        Parameter sourceFile = Parameter("./Tests/smalltest.txt");
        component(rKey).parameterPort("INFILE").receive(sourceFile);
        Parameter lower = Parameter(3);
        component(sKey).parameterPort("LSIZE").receive(lower);
        Parameter upper = Parameter(7);
        component(sKey).parameterPort("USIZE").receive(upper);
        // should be true if everything is ok
        return isWiredUp;
    }
    
}