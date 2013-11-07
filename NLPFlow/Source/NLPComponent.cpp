//
//  NLPComponent.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPComponent.hpp"
#include <boost/tokenizer.hpp>
#include <fstream>
#include <sstream>

namespace NLP {
    
# pragma mark - Component methods
    
    Component::Component(std::string name) : mName(name) {}
    
    const std::string Component::name() const {
        return mName;
    }
    
    InputPort& Component::inputPort(std::string name) {
        return mInputs.at(name);
    }
    
    OutputPort& Component::outputPort(std::string name) {
        return mOutputs.at(name);
    }
    
    bool Component::hasInput(std::string name) {
        if(mInputs.find(name) != mInputs.end()) {
            return true;
        }
        return false;
    }
    
    bool Component::hasOutput(std::string name) {
        if(mOutputs.find(name) != mOutputs.end()) {
            return true;
        }
        return false;
    }
    
    bool Component::hasOpenPorts() {
        // check for open inputs
        for(auto it = mInputs.begin(); it != mInputs.end(); ++it) {
            if(it->second->isOpen()) {
                return true;
            }
        }
        // check for open outputs
        for(auto it = mOutputs.begin(); it != mOutputs.end(); ++it) {
            if(it->second->isOpen()) {
                return true;
            }
        }
        return false;
    }
    
    
# pragma mark - TextReader methods
    
    TextReader::TextReader(std::string name) : Component(name) {
        // no input ports; only a single output port
        std::string outName = "TXTOUT";
        mOutputs.insert(outName,new OutputPort("TXTOUT",Port::TYPE_STR));
    }
    
    void TextReader::execute() {
        // temporary hack - eventually want to send things in a parameter port
        std::string fileName = "./Tests/smalltest.txt";
        std::ifstream handle(fileName.c_str(), std::ios_base::in);
        std::string line;
        if (!handle.good()) {
            std::cout << "There's a problem with your file." << std::endl;
        }
        while(handle.good()) {
            std::getline(handle,line);
            packet_t output = line;
            mOutputs.at("TXTOUT").send(output);
        }
        return;
    }
    
# pragma mark - TextWriter methods
    
    TextWriter::TextWriter(std::string name) : Component(name) {
        // has no output port; only a single input port
        std::string inName = "TXTIN";
        mInputs.insert(inName,new InputPort("TXTIN",Port::TYPE_STR));
    }
    
    void TextWriter::execute() {
        // process packet
        std::string toWrite;
        // empty the input queue
        while(mInputs.at("TXTIN").packetsReady()) {
            mInputs.at("TXTIN").dropNextPacket(toWrite);
            std::cout << toWrite << std::endl;
        }
        return;
    }
    
# pragma mark - IntWriter methods
    
    IntWriter::IntWriter(std::string name) : Component(name) {
        // one input, no outputs
        std::string inName = "INTIN";
        mInputs.insert(inName,new InputPort("INTIN",Port::TYPE_INT));
    }
    
    void IntWriter::execute() {
        int toWrite;
        // empty the queue
        while(mInputs.at("INTIN").packetsReady()) {
            mInputs.at("INTIN").dropNextPacket(toWrite);
            std::cout << "Integer packet value = " << toWrite << std::endl;
        }
        return;
    }
    
# pragma mark - Tokenizer methods
    
    Tokenizer::Tokenizer(std::string name) : Component(name) {
        // one input, one output
        std::string inName = "TXTIN";
        std::string outName = "TXTOUT";
        mInputs.insert(inName,new InputPort("TXTIN",Port::TYPE_STR));
        mOutputs.insert(outName,new OutputPort("TXTOUT",Port::TYPE_STR));
    }
    
    void Tokenizer::execute() {
        // for tokenizing
        boost::char_separator<char> sep(", \t\n");
        // process packets
        std::string toTokenize;
        // empty the input queue and tokenize one at a time
        while(mInputs.at("TXTIN").packetsReady()) {
            mInputs.at("TXTIN").dropNextPacket(toTokenize);
            // tokenize
            boost::tokenizer<boost::char_separator<char>> tokenizer(toTokenize,sep);
            // throw the tokens
            for(const auto& t : tokenizer) {
                packet_t p = std::string(t);
                mOutputs.at("TXTOUT").send(p);
            }
            
        }
    }
    
# pragma mark - BinaryDuplicator methods
    
    BinaryStringDuplicator::BinaryStringDuplicator(std::string name) : Component(name) {
        // one input, two outputs
        std::string inName = "IN";
        std::string out1 = "OUT1";
        std::string out2 = "OUT2";
        mInputs.insert(inName,new InputPort("IN",Port::TYPE_STR));
        mOutputs.insert(out1,new OutputPort("OUT1",Port::TYPE_STR));
        mOutputs.insert(out2,new OutputPort("OUT2",Port::TYPE_STR));
    }
    
    void BinaryStringDuplicator::execute() {
        // process packets
        std::string toSend;
        while(mInputs.at("IN").packetsReady()) {
            mInputs.at("IN").dropNextPacket(toSend);
            packet_t p1 = toSend;
            packet_t p2 = toSend;
            mOutputs.at("OUT1").send(p1);
            mOutputs.at("OUT2").send(p2);
        }
        return;
    }
    
# pragma mark - StringCounter methods
    
    StringCounter::StringCounter(std::string name) : Component(name) {
        // one string input, one integer output
        std::string inName = "TXTIN";
        std::string outName = "INTOUT";
        mInputs.insert(inName, new InputPort("TXTIN",Port::TYPE_STR));
        mOutputs.insert(outName, new OutputPort("INTOUT",Port::TYPE_INT));
        mCount = 0;
    }
    
    void StringCounter::execute() {
        std::string toCount;
        while(mInputs.at("TXTIN").packetsReady()) {
            mInputs.at("TXTIN").dropNextPacket(toCount);
            mCount += 1;
        }
        // now throw the count
        packet_t p = mCount;
        mOutputs.at("INTOUT").send(p);
        return;
    }
    
}
