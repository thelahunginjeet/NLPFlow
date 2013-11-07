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
#include <boost/ptr_container/ptr_map.hpp>

namespace NLP {
    
    // abstract base class
    class Component {
    public:
        Component(std::string name = "BLACKBOX");
        const std::string name() const;
        InputPort& inputPort(std::string name);
        OutputPort& outputPort(std::string name);
        bool hasOpenPorts();

        virtual void execute() = 0;
        
    protected:
        std::string mName;
        boost::ptr_map<std::string,OutputPort> mOutputs;
        boost::ptr_map<std::string,InputPort> mInputs;
    };
    
    
    // specific components
    // reader
    class TextReader : public Component {
    public:
        TextReader(std::string name = "TextReader");
        void execute();
    };
    
    // writers
    class TextWriter : public Component {
    public:
        TextWriter(std::string name = "TextWriter");
        void execute();
    };
    
    class IntWriter : public Component {
    public:
        IntWriter(std::string name = "IntegerWriter");
        void execute();
    };
    
    class Tokenizer : public Component {
    public:
        Tokenizer(std::string name = "Tokenizer");
        void execute();
    };
    
    class BinaryStringDuplicator : public Component {
    public:
        BinaryStringDuplicator(std::string name = "StringDuplicator");
        void execute();
    };
    
    class StringCounter : public Component {
    public:
        StringCounter(std::string name = "Counter");
        void execute();
    private:
        int mCount;
    };
    
}

#endif
