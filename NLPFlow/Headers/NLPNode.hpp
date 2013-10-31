//
//  NLPNode.h
//  NLPFlow
//
//  Created by Kevin Brown on 10/22/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPNode_hpp
#define NLP_NLPNode_hpp

#include "NLPPort.hpp"
#include <boost/signals2/signal.hpp>

namespace NLP {
    
    class Node {
    public:
        typedef boost::signals2::signal<void (std::string, packet_t)> Emitter;
        typedef boost::signals2::connection Wire;
        
        Node();
        virtual bool connectPort(Node& node, std::string sendPort, std::string recPort) = 0;
        virtual bool canConnectPort(Node& node, std::string sendPort, std::string recPort) = 0;
        virtual bool disconnect(std::string outPort) = 0;
        virtual void send(std::string sendPort, packet_t output) = 0;
        virtual void receive(std::string recPort, packet_t input) = 0;
    };
    
    // TextReader has a single output and no non-parameter inputs
    class TextReaderNode : public Node {
    public:
        TextReaderNode();
        virtual bool connectPort(Node& node, std::string sendPort, std::string recPort);
        virtual bool canConnectPort(Node& node, std::string sendPort, std::string recPort);
        virtual bool disconnect(std::string outPort);
        virtual void send(std::string sendPort, packet_t output);
        virtual void receive(std::string sendPort, packet_t input) {};
        void read(const std::string& fileName);
    protected:
        Emitter emitter;
        Wire wire;
        
    private:
        std::string mFileName;
    };
    
    // TextWriter has a single input and no non-parameter outputs (it does not need to be a signal)
    class StringWriterNode : public Node {
    public:
        StringWriterNode();
        virtual bool connectPort(Node& node, std::string sendPort, std::string recPort) {return false;};
        virtual bool canConnectPort(Node& node, std::string sendPort, std::string recPort) {return false;};
        virtual bool disconnect(std::string outPort) {return false;};
        virtual void send(std::string sendPort, packet_t output) {};
        virtual void receive(std::string sendPort, packet_t input);
        void write(packet_t input);
    
    private:
        std::string mFileName;
    };
    
    // Tokenizer has a single input and a single non-parameter output; it streams tokens
    class Tokenizer : public Node {
    
    public:
        Tokenizer();
        virtual bool connectPort(Node& node, std::string sendPort, std::string recPort);
        virtual bool canConnectPort(Node& node, std::string sendPort, std::string recPort);
        virtual bool disconnect(std::string outPort);
        virtual void send(std::string sendPort, packet_t output);
        virtual void receive(std::string recPort, packet_t input);
        virtual void tokenize(std::string& input);
    
    protected:
        Emitter emitter;
        Wire wire;
    };
    
    
} // end namespace

#endif
