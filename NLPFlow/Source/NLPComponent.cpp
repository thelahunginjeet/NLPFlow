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
        // log and parameter ports have fixed names/types (they cannot be
        //  connected to anything anyway)
        mLogPort = LogPort();
        mErrorPort = ErrorPort();
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
    
    ParameterPort& Component::parameterPort(std::string name) {
        return mParameters.at(name);
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
        // we also need to make sure an initial box (params but no inputs) can still run
        // parOpen is true if ALL parameter ports are open
        // parOpen is also true if the box has no parameter ports
        // if the only open ports are parameter ports, the network will not hang
        bool parOpen = false;
        bool inOpen = false;
        bool outOpen = false;
        // check parameter ports
        if(mParameters.size()) {
            for(auto it = mParameters.begin(); it != mParameters.end(); ++it) {
                if(it->second->isOpen()) {
                    parOpen = true;
                    break;
                }
            }
        } else {
            parOpen = true;
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
        std::string parName = "INFILE";
        mParameters.insert(parName, new ParameterPort("INFILE",Port::TYPE_STR));
        // no input ports; only a single output port
        std::string outName = "TXTOUT";
        mOutputs.insert(outName,new OutputPort("TXTOUT",Port::TYPE_STR));
    }
    
    void TextReader::execute() {
        mLogPort.send("TextReader.execute()\n");
        // get the read location from the parameter port
        std::string fileName = mParameters.at("INFILE").parameter().fetchParameterData<std::string>();
        std::ifstream handle(fileName.c_str(), std::ios_base::in);
        std::string line;
        if (!handle.good()) {
            mErrorPort.send("There's a problem with your file.\n");
        }
        mOutputs.at("TXTOUT").open();
        while(handle.good()) {
            std::getline(handle,line);
            Packet output = Packet(line);
            mOutputs.at("TXTOUT").send(output);
        }
        // close the ports
        mOutputs.at("TXTOUT").close();
        mParameters.at("INFILE").close();
        return;
    }
    
# pragma mark - TextWriter methods
    
    TextWriter::TextWriter(std::string name) : Component(name) {
        // has no output port; only a single input port
        std::string inName = "TXTIN";
        mInputs.insert(inName,new InputPort("TXTIN",Port::TYPE_STR));
    }
    
    void TextWriter::execute() {
        mLogPort.send("TextWriter.execute()\n");
        // process packet
        Packet toWrite;
        // empty the input queue
        while(mInputs.at("TXTIN").isOpen()) {
            mInputs.at("TXTIN").dropNextPacket(toWrite);
            mLogPort.send(toWrite.fetchPacketData<std::string>());
            mLogPort.send("\n");
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
        mLogPort.send("IntWriter.execute()\n");
        std::stringstream stream;
        Packet toWrite;
        // empty the queue
        while(mInputs.at("INTIN").isOpen()) {
            mInputs.at("INTIN").dropNextPacket(toWrite);
            stream << "Integer packet value = " << toWrite.fetchPacketData<int>() << std::endl;
            mLogPort.send(stream.str());
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
        mLogPort.send("Tokenizer.execute()\n");
        // for tokenizing
        boost::char_separator<char> sep(", \t\n");
        // process packets
        Packet toTokenize;
        // empty the input queue and tokenize one at a time
        while(mInputs.at("TXTIN").isOpen()) {
            mInputs.at("TXTIN").dropNextPacket(toTokenize);
            // tokenize
            boost::tokenizer<boost::char_separator<char>> tokenizer(toTokenize.fetchPacketData<std::string>(),sep);
            // throw the tokens
            mOutputs.at("TXTOUT").open();
            for(const auto& t : tokenizer) {
                Packet p = Packet(std::string(t));
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
        mLogPort.send("BinaryStringDuplicator.execute()\n");
        // process packets
        Packet toSend;
        while(mInputs.at("IN").isOpen()) {
            mOutputs.at("OUT1").open();
            mOutputs.at("OUT1").open();
            mInputs.at("IN").dropNextPacket(toSend);
            Packet p1 = Packet(toSend);
            Packet p2 = Packet(toSend);
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
        std::string paramPort = "SIZE";
        mInputs.insert(inName,new InputPort("IN",Port::TYPE_STR));
        mOutputs.insert(trueOut,new OutputPort("OUT_T",Port::TYPE_STR));
        mOutputs.insert(falseOut,new OutputPort("OUT_F",Port::TYPE_STR));
        mParameters.insert(paramPort, new ParameterPort("SIZE",Port::TYPE_INT));
    }
    
    void StringSizeSelector::execute() {
        mLogPort.send("StringSizeSelector.execute()\n");
        // read the size parameter
        int threshold = mParameters.at("SIZE").parameter().fetchParameterData<int>();
        // process packets
        Packet p;
        while(mInputs.at("IN").isOpen()) {
            mInputs.at("IN").dropNextPacket(p);
            std::string data = p.fetchPacketData<std::string>();
            if (data.size() <= threshold) {
                mOutputs.at("OUT_T").open();
                mOutputs.at("OUT_T").send(p);
                mOutputs.at("OUT_T").close();
            } else {
                mOutputs.at("OUT_F").open();
                mOutputs.at("OUT_F").send(p);
                mOutputs.at("OUT_F").close();
            }
        }
        mParameters.at("SIZE").close();
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
        mLogPort.send("StringCounter.execute()\n");
        Packet toCount;
        while(mInputs.at("TXTIN").isOpen()) {
            mInputs.at("TXTIN").dropNextPacket(toCount);
            mCount += 1;
        }
        // now throw the count
        Packet p = Packet(mCount);
        mOutputs.at("INTOUT").open();
        mOutputs.at("INTOUT").send(p);
        mOutputs.at("INTOUT").close();
        return;
    }
    
}
