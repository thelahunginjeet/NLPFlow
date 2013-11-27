//
//  NLPComponent.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPComponent.hpp"
#include <boost/tokenizer.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <fstream>
#include <sstream>

namespace NLP {
    
# pragma mark - Wrapper for Component
    
    struct Component::Wrapper {
        Wrapper(std::string name);
        std::string mName;
        boost::ptr_map<std::string,OutputPort> mOutputs;
        boost::ptr_map<std::string,InputPort> mInputs;
        boost::ptr_map<std::string,ParameterPort> mParameters;
        ErrorPort mErrorPort;
        LogPort mLogPort;
    };
    
    Component::Wrapper::Wrapper(std::string name) : mName(name) {
        mLogPort = LogPort();
        mErrorPort = ErrorPort();
    }
    
# pragma mark - Component methods
    
    Component::Component(std::string name) : mWrapper(new Wrapper(name)) {}
    
    const std::string Component::name() const {
        return mWrapper->mName;
    }
    
    InputPort& Component::inputPort(std::string name) {
        return (mWrapper->mInputs).at(name);
    }
    
    OutputPort& Component::outputPort(std::string name) {
        return (mWrapper->mOutputs).at(name);
    }
    
    ParameterPort& Component::parameterPort(std::string name) {
        return (mWrapper->mParameters).at(name);
    }
    
    LogPort& Component::logPort() {
        return mWrapper->mLogPort;
    }
    
    ErrorPort& Component::errorPort() {
        return mWrapper->mErrorPort;
    }
    
    void Component::addPort(std::string portName, PORT_ID portID, Port::PORT_TYPE portType) {
        switch (portID) {
            case PORT_IN:
                (mWrapper->mInputs).insert(portName,new InputPort(portName,portType));
                break;
            case PORT_OUT:
                (mWrapper->mOutputs).insert(portName,new OutputPort(portName,portType));
                break;
            case PORT_PAR:
                (mWrapper->mParameters).insert(portName,new ParameterPort(portName,portType));
                break;
            default:
                std::cout << "SHOULD NEVER GET HERE!!" << std::endl;
                break;
        }
    }
    
    bool Component::hasInput(std::string name) {
        if((mWrapper->mInputs).find(name) != (mWrapper->mInputs).end()) {
            return true;
        }
        return false;
    }
    
    bool Component::hasOutput(std::string name) {
        if((mWrapper->mOutputs).find(name) != (mWrapper->mOutputs).end()) {
            return true;
        }
        return false;
    }
    
    bool Component::hasParameter(std::string name) {
        if((mWrapper->mParameters).find(name) != (mWrapper->mParameters).end()) {
            return true;
        }
        return false;
    }
    
    
    const bool Component::hasOpenPorts() const {
        // component is run if:
        //  -ALL parameter AND ALL inputs are open OR outputs are open
        //  -if the box has no inputs but a parameter, it runs
        //  -parOpen is true if ALL parameter ports are open
        //  -inOpen is true if ALL input ports are open
        //  -parOpen is also true if the box has no parameter ports
        //  -if the only open ports are parameter ports, the network will not hang
        bool parOpen = true;
        bool inOpen = true;
        bool outOpen = false;
        // check parameter ports
        if((mWrapper->mParameters).size()) {
            for(auto it = (mWrapper->mParameters).begin(); it != (mWrapper->mParameters).end(); ++it) {
                if(it->second->isOpen()) {
                    parOpen = parOpen && true;
                } else {
                    parOpen = false;
                    break;
                }
            }
        } else {
            parOpen = true;
        }
        // input checking
        if(mWrapper->mInputs.size()) {
            for(auto it = (mWrapper->mInputs).begin(); it != (mWrapper->mInputs).end(); ++it) {
                if(it->second->isOpen()) {
                    inOpen = inOpen && true;
                } else {
                    inOpen = false;
                    break;
                }
            }
        } else {
            inOpen = true;
        }
        // check for open outputs
        for(auto it = (mWrapper->mOutputs).begin(); it != (mWrapper->mOutputs).end(); ++it) {
            if(it->second->isOpen()) {
                outOpen = true;
                break;
            }
        }
        return (parOpen && inOpen) || outOpen;
    }
    
    
    const std::string Component::str() const {
        std::stringstream s;
        s << "<Component:" << name() << " <" << this << "> state: ";
        if(hasOpenPorts()) {
            s << "OPEN >";
        } else {
            s << "CLOSED > ";
        }
        return s.str();
    }
    
    
# pragma mark - TextReader methods
    
    TextReader::TextReader(std::string name) : Component(name) {
        // parameter port that grabs the filename
        std::string parName = "INFILE";
        addPort(parName,PORT_PAR,Port::TYPE_STR);
        // output port that throws the text
        std::string outName = "TXTOUT";
        addPort(outName,PORT_OUT,Port::TYPE_STR);
    }
    
    void TextReader::execute() {
        logPort().send("TextReader.execute()\n");
        // get the read location from the parameter port
        std::string fileName = parameterPort("INFILE").parameter().fetchParameterData<std::string>();
        std::ifstream handle(fileName.c_str(), std::ios_base::in);
        std::string line;
        if (!handle.good()) {
            errorPort().send("There's a problem with your file.\n");
        }
        outputPort("TXTOUT").open();
        while(handle.good()) {
            std::getline(handle,line);
            Packet output = Packet(line);
            outputPort("TXTOUT").send(output);
        }
        // close the ports
        outputPort("TXTOUT").close();
        parameterPort("INFILE").close();
        return;
    }
    
    
# pragma mark - PacketWriter methods
    
    PacketWriter::PacketWriter(std::string name) : Component(name) {
        // one input, no outputs
        std::string inName = "PACKIN";
        addPort(inName,PORT_IN,Port::TYPE_ANY);
    }
    
    void PacketWriter::execute() {
        logPort().send("PacketWriter.execute()\n");
        Packet toWrite;
        // empty the queue
        while(inputPort("PACKIN").isOpen()) {
            inputPort("PACKIN").dropNextPacket(toWrite);
            logPort().send(toWrite.str());
            logPort().send("\n");
        }
        return;
    }
    
    
# pragma mark - Tokenizer methods
    
    Tokenizer::Tokenizer(std::string name) : Component(name) {
        // one input, one output
        std::string inName = "TXTIN";
        addPort(inName,PORT_IN,Port::TYPE_STR);
        std::string outName = "TXTOUT";
        addPort(outName,PORT_OUT,Port::TYPE_STR);
    }
    
    void Tokenizer::execute() {
        logPort().send("Tokenizer.execute()\n");
        // for tokenizing
        boost::char_separator<char> sep(", \t\n");
        // process packets
        Packet toTokenize;
        // empty the input queue and tokenize one at a time
        while(inputPort("TXTIN").isOpen()) {
            inputPort("TXTIN").dropNextPacket(toTokenize);
            // tokenize
            boost::tokenizer<boost::char_separator<char>> tokenizer(toTokenize.fetchPacketData<std::string>(),sep);
            // throw the tokens
            outputPort("TXTOUT").open();
            for(const auto& t : tokenizer) {
                Packet p = Packet(std::string(t));
                outputPort("TXTOUT").send(p);
            }
            outputPort("TXTOUT").close();
        }
        return;
    }
    
# pragma mark - BinaryDuplicator methods
    
    BinaryStringDuplicator::BinaryStringDuplicator(std::string name) : Component(name) {
        // one input, two outputs
        std::string inName = "IN";
        addPort(inName, PORT_IN, Port::TYPE_STR);
        std::string out1 = "OUT1";
        addPort(out1, PORT_OUT, Port::TYPE_STR);
        std::string out2 = "OUT2";
        addPort(out2, PORT_OUT, Port::TYPE_STR);
    }
    
    void BinaryStringDuplicator::execute() {
        logPort().send("BinaryStringDuplicator.execute()\n");
        // process packets
        Packet toSend;
        while(inputPort("IN").isOpen()) {
            outputPort("OUT1").open();
            outputPort("OUT1").open();
            inputPort("IN").dropNextPacket(toSend);
            Packet p1 = Packet(toSend);
            Packet p2 = Packet(toSend);
            outputPort("OUT1").send(p1);
            outputPort("OUT2").send(p2);
            outputPort("OUT1").close();
            outputPort("OUT2").close();
        }
        return;
    }
    
# pragma mark - StringSizeSelector methods
    
    StringSizeSelector::StringSizeSelector(std::string name) : Component(name) {
        // one input, two outputs, parametric size selector
        std::string inName = "IN";
        addPort(inName, PORT_IN, Port::TYPE_STR);
        std::string trueOut = "OUT_T";
        addPort(trueOut, PORT_OUT, Port::TYPE_STR);
        std::string falseOut = "OUT_F";
        addPort(falseOut, PORT_OUT, Port::TYPE_STR);
        std::string paramName = "SIZE";
        addPort(paramName, PORT_PAR, Port::TYPE_INT);
    }
    
    void StringSizeSelector::execute() {
        logPort().send("StringSizeSelector.execute()\n");
        // read the size parameter
        int threshold = parameterPort("SIZE").parameter().fetchParameterData<int>();
        // process packets
        Packet p;
        while(inputPort("IN").isOpen()) {
            inputPort("IN").dropNextPacket(p);
            std::string data = p.fetchPacketData<std::string>();
            if (data.size() <= threshold) {
                outputPort("OUT_T").open();
                outputPort("OUT_T").send(p);
                outputPort("OUT_T").close();
            } else {
                outputPort("OUT_F").open();
                outputPort("OUT_F").send(p);
                outputPort("OUT_F").close();
            }
        }
        parameterPort("SIZE").close();
    }
    
# pragma mark - StringCounter methods
    
    StringCounter::StringCounter(std::string name) : Component(name) {
        // one string input, one integer output
        std::string inName = "TXTIN";
        addPort(inName, PORT_IN, Port::TYPE_STR);
        std::string outName = "INTOUT";
        addPort(outName, PORT_OUT, Port::TYPE_INT);
        mCount = 0;
    }
    
    void StringCounter::execute() {
        logPort().send("StringCounter.execute()\n");
        Packet toCount;
        while(inputPort("TXTIN").isOpen()) {
            inputPort("TXTIN").dropNextPacket(toCount);
            mCount += 1;
        }
        // now throw the count
        Packet p = Packet(mCount);
        outputPort("INTOUT").open();
        outputPort("INTOUT").send(p);
        outputPort("INTOUT").close();
        return;
    }
    
}
