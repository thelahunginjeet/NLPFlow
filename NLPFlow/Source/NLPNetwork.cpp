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
    // This should check everyting - boxes existing, boxes containing the named ports, and port type (checked in OutputPort.connect())
    {
        auto it1 = mBoxes.find(boxOut);
        if (it1 == mBoxes.end()) {
            return false;
        }
        auto it2 = mBoxes.find(boxIn);
        if (it2 == mBoxes.end()) {
            return false;
        }
        if ((it1->second)->hasOutput(outName) && (it2->second)->hasInput(inName)) {
            return (it1->second)->outputPort(outName).connect((it2->second)->inputPort(inName));
        }
        return false;
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
        std::cout << "<FLOW START>" << std::endl;
        // this runs the network until there is nothing to do
        while(isRunnable()) {
            // a lot of these statements are just for debugging
            std::cout << "In run loop . . ." << std::endl;
            // iterate over all the components and to run them
            for(auto it = mBoxes.begin(); it != mBoxes.end(); ++it) {
                std::cout << "Component '"  << (it->first) << "' in state ";
                std::string state;
                if((it->second)->hasOpenPorts()) {
                    state = "OPEN";
                } else {
                    state = "CLOSED";
                }
                std::cout << state << std::endl;
                if((it->second)->hasOpenPorts()) {
                    (it->second)->execute();
                }
            }
        }
        std:: cout << "<FLOW END>" << std::endl;
    }
    
# pragma mark - NLPNetwork methods
    
    NLPNetwork::NLPNetwork() : Network() {}
    
    bool NLPNetwork::define() {
        // set up the boxes
        std::string reader = "READER";
        mBoxes.insert(reader, new TextReader("TextReader"));
        std::string tokenizer = "TOKENIZER";
        mBoxes.insert(tokenizer, new Tokenizer("Tokenizer"));
        std::string selector = "SELECTOR";
        mBoxes.insert(selector, new StringSizeSelector("Selector"));
        std::string counter = "COUNTER";
        mBoxes.insert(counter, new StringCounter("Counter"));
        std::string intwriter = "INTWRITER";
        mBoxes.insert(intwriter, new IntWriter("IntWriter"));
        std::string txtwriter = "TXTWRITER";
        mBoxes.insert(txtwriter, new TextWriter("TxtWriter"));
        // connectivity
        bool isWiredUp = true;
        isWiredUp = isWiredUp && connect(reader,"TXTOUT",tokenizer,"TXTIN");
        isWiredUp = isWiredUp && connect(tokenizer,"TXTOUT",selector,"IN");
        isWiredUp = isWiredUp && connect(selector,"OUT_T",txtwriter,"TXTIN");
        isWiredUp = isWiredUp && connect(selector,"OUT_F",counter,"TXTIN");
        isWiredUp = isWiredUp && connect(counter,"INTOUT",intwriter,"INTIN");
        // parameters to boxes that need them
        packet_t sourceFile = "./Tests/smalltest.txt";
        mBoxes.at("READER").parameterPort().receive(sourceFile);
        packet_t threshold = 3;
        mBoxes.at("SELECTOR").parameterPort().receive(threshold);
        // should be true if everything is ok
        return isWiredUp;
    }
    
}