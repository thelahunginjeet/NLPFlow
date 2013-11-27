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
    
    // abstract base class + pimpl interface
    class Component {
    public:
        Component(std::string name = "BLACKBOX");
        const std::string name() const;
        InputPort& inputPort(std::string name);
        OutputPort& outputPort(std::string name);
        ParameterPort& parameterPort(std::string name);
        LogPort& logPort();
        ErrorPort& errorPort();
        
        enum PORT_ID {
            PORT_IN, PORT_OUT, PORT_PAR
        };
        
        void addPort(std::string portName, PORT_ID portID, Port::PORT_TYPE portType);
        
        bool hasInput(std::string name);
        bool hasOutput(std::string name);
        bool hasParameter(std::string name);
        
        const bool hasOpenPorts() const;

        virtual void execute() = 0;
        
        const std::string str() const;
        
    private:
        struct Wrapper;
        std::shared_ptr<Wrapper> mWrapper;
    };
    
    
    // specific components
    // reader
    class TextReader : public Component {
    public:
        TextReader(std::string name = "TextReader");
        void execute();
    };
    
    // writers
    class PacketWriter : public Component {
    public:
        PacketWriter(std::string name = "PacketWriter");
        void execute();
    };
    
    // tokenizer
    class Tokenizer : public Component {
    public:
        Tokenizer(std::string name = "Tokenizer");
        void execute();
    };
    
    // splitter that sends the same output to two different locations
    class BinaryStringDuplicator : public Component {
    public:
        BinaryStringDuplicator(std::string name = "StringDuplicator");
        void execute();
    };
    
    // selector that sends strings <= size to one output and > to another
    class StringSizeSelector : public Component {
    public:
        StringSizeSelector(std::string name = "SizeSelector");
        void execute();
    };
    
    // counts its inputs
    class StringCounter : public Component {
    public:
        StringCounter(std::string name = "Counter");
        void execute();
    private:
        int mCount;
    };
    
}

#endif
