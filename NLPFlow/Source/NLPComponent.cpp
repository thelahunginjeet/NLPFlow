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
    
    Component::Component(std::string name) : mName(name) {
        // default parameter port with TYPE_NULL
        mParamPort = ParameterPort("EMPTY",Port::TYPE_NULL);
    }
    
    const std::string Component::name() const {
        return mName;
    }
    
    InputPort& Component::inputPort(std::string name) {
        return mInputs.at(name);
    }
    
    OutputPort& Component::outputPort(std::string name) {
        return mOutputs.at(name);
    }
    
    ParameterPort& Component::parameterPort() {
        return mParamPort;
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
        // component is run if the parameter AND inputs are open OR the outputs are open
        // however, have to make sure an initial box (param but no input) can still run.
        // if only parameter ports are open in components which also take inputs, the
        //  network will not lock
        bool parOpen = false;
        bool inOpen = false;
        bool outOpen = false;
        // parameter checking; only use the real value if the parameter port has a real type
        if(mParamPort.type() == Port::TYPE_NULL) {
            parOpen = true;
        }
        else {
            parOpen = mParamPort.isOpen();
        }
        // input checking
        if(mInputs.size()) {
            for(auto it = mInputs.begin(); it != mInputs.end(); ++it) {
                if(it->second->isOpen()) {
                    inOpen = true;
                    break;
                }
            }
        } else {
            inOpen = true;
        }
        // check for open outputs
        for(auto it = mOutputs.begin(); it != mOutputs.end(); ++it) {
            if(it->second->isOpen()) {
                outOpen = true;
                break;
            }
        }
        return (parOpen && inOpen) || outOpen;
    }
    
    
# pragma mark - TextReader methods
    
    TextReader::TextReader(std::string name) : Component(name) {
        // parameter port that grabs the filename
        mParamPort = ParameterPort("FILENAME",Port::TYPE_STR);
        // no input ports; only a single output port
        std::string outName = "TXTOUT";
        mOutputs.insert(outName,new OutputPort("TXTOUT",Port::TYPE_STR));
    }
    
    void TextReader::execute() {
        std::cout << "TextReader.execute()" << std::endl;
        // get the read location from the parameter port
        std::string fileName = mParamPort.fetchParameter<std::string>();
        std::ifstream handle(fileName.c_str(), std::ios_base::in);
        std::string line;
        if (!handle.good()) {
            std::cout << "There's a problem with your file." << std::endl;
        }
        mOutputs.at("TXTOUT").open();
        while(handle.good()) {
            std::getline(handle,line);
            packet_t output = line;
            mOutputs.at("TXTOUT").send(output);
        }
        // close the ports
        mOutputs.at("TXTOUT").close();
        mParamPort.close();
        return;
    }
    
# pragma mark - TextWriter methods
    
    TextWriter::TextWriter(std::string name) : Component(name) {
        // has no output port; only a single input port
        std::string inName = "TXTIN";
        mInputs.insert(inName,new InputPort("TXTIN",Port::TYPE_STR));
    }
    
    void TextWriter::execute() {
        std::cout << "TextWriter.execute()" << std::endl;
        // process packet
        std::string toWrite;
        // empty the input queue
        while(mInputs.at("TXTIN").isOpen()) {
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
        std::cout << "IntWriter.execute()" << std::endl;
        int toWrite;
        // empty the queue
        while(mInputs.at("INTIN").isOpen()) {
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
        std::cout << "Tokenizer.execute()" << std::endl;
        // for tokenizing
        boost::char_separator<char> sep(", \t\n");
        // process packets
        std::string toTokenize;
        // empty the input queue and tokenize one at a time
        while(mInputs.at("TXTIN").isOpen()) {
            mInputs.at("TXTIN").dropNextPacket(toTokenize);
            // tokenize
            boost::tokenizer<boost::char_separator<char>> tokenizer(toTokenize,sep);
            // throw the tokens
            mOutputs.at("TXTOUT").open();
            for(const auto& t : tokenizer) {
                packet_t p = std::string(t);
                mOutputs.at("TXTOUT").send(p);
            }
            mOutputs.at("TXTOUT").close();
        }
        return;
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
        std::cout << "BinaryStringDuplicator.execute()" << std::endl;
        // process packets
        std::string toSend;
        while(mInputs.at("IN").isOpen()) {
            mOutputs.at("OUT1").open();
            mOutputs.at("OUT1").open();
            mInputs.at("IN").dropNextPacket(toSend);
            packet_t p1 = toSend;
            packet_t p2 = toSend;
            mOutputs.at("OUT1").send(p1);
            mOutputs.at("OUT2").send(p2);
            mOutputs.at("OUT1").close();
            mOutputs.at("OUT2").close();
        }
        return;
    }
    
# pragma mark - StringSizeSelector methods
    
    StringSizeSelector::StringSizeSelector(std::string name) : Component(name) {
        // one input, two outputs, parametric size selector
        std::string inName = "IN";
        std::string trueOut = "OUT_T";
        std::string falseOut = "OUT_F";
        mInputs.insert(inName,new InputPort("IN",Port::TYPE_STR));
        mOutputs.insert(trueOut,new OutputPort("OUT_T",Port::TYPE_STR));
        mOutputs.insert(falseOut,new OutputPort("OUT_F",Port::TYPE_STR));
        mParamPort = ParameterPort("SIZE",Port::TYPE_INT);
    }
    
    void StringSizeSelector::execute() {
        std::cout << "StringSizeSelector.execute()" << std::endl;
        // read the size parameter
        int threshold = mParamPort.fetchParameter<int>();        // process packets
        std::string toSend;
        while(mInputs.at("IN").isOpen()) {
            mInputs.at("IN").dropNextPacket(toSend);
            packet_t p = toSend;
            if (toSend.size() <= threshold) {
                mOutputs.at("OUT_T").open();
                mOutputs.at("OUT_T").send(p);
                mOutputs.at("OUT_T").close();
            } else {
                mOutputs.at("OUT_F").open();
                mOutputs.at("OUT_F").send(p);
                mOutputs.at("OUT_F").close();
            }
        }
        mParamPort.close();
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
        std::cout << "StringCounter.execute()" << std::endl;
        std::string toCount;
        while(mInputs.at("TXTIN").isOpen()) {
            mInputs.at("TXTIN").dropNextPacket(toCount);
            mCount += 1;
        }
        // now throw the count
        packet_t p = mCount;
        mOutputs.at("INTOUT").open();
        mOutputs.at("INTOUT").send(p);
        mOutputs.at("INTOUT").close();
        return;
    }
    
}
