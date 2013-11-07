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
        // need to check here
        auto it1 = mBoxes.find(boxOut);
        auto it2 = mBoxes.find(boxIn);
        // attempt the connect
        return (it1->second)->outputPort(outName).connect((it2->second)->inputPort(inName));
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
        // HACK TO MAKE THINGS GO
        mBoxes.at("READER").execute();
        mBoxes.at("TOKENIZER").execute();
        mBoxes.at("SPLITTER").execute();
        mBoxes.at("TXTWRITER").execute();
        mBoxes.at("COUNTER").execute();
        mBoxes.at("INTWRITER").execute();
        // This is where we want to go
        /*
        // isRunnable indicates we are capable of running
        while(isRunnable()) {
            // iterate over all the components and try execution
            for(auto it = mBoxes.begin(); it != mBoxes.end(); ++it) {
                (it->second)->execute();
            }
        }
         */
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
        std::string splitter = "SPLITTER";
        mBoxes.insert(splitter, new BinaryStringDuplicator("Splitter"));
        std::string counter = "COUNTER";
        mBoxes.insert(counter, new StringCounter("Counter"));
        std::string intwriter = "INTWRITER";
        mBoxes.insert(intwriter, new IntWriter("INTWRITER"));
        std::string txtwriter = "TXTWRITER";
        mBoxes.insert(txtwriter, new TextWriter("TXTWRITER"));
        // connectivity
        bool isWiredUp = true;
        isWiredUp = isWiredUp && connect(reader,"TXTOUT",tokenizer,"TXTIN");
        isWiredUp = isWiredUp && connect(tokenizer,"TXTOUT",splitter,"IN");
        isWiredUp = isWiredUp && connect(splitter,"OUT1",txtwriter,"TXTIN");
        isWiredUp = isWiredUp && connect(splitter,"OUT2",counter,"TXTIN");
        isWiredUp = isWiredUp && connect(counter,"INTOUT",intwriter,"INTIN");
        // should be true if everything is ok
        return isWiredUp;
    }
    
    void NLPNetwork::test() {

        
    }
    
}