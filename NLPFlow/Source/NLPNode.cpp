//
//  NLPNode.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/22/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>
#include "NLPNode.hpp"

namespace NLP {
    
    # pragma mark - Node methods
    Node::Node() {}
    
    # pragma mark - TextReaderNode methods
    
    TextReaderNode::TextReaderNode() : Node (), mFileName("NONE") {}
    
    bool TextReaderNode::connectPort(Node &node, std::string sendPort, std::string recPort) {
         if(canConnectPort(node,sendPort,recPort)) {
             wire.disconnect();
             wire = emitter.connect(boost::bind(&Node::receive, &node, ::_1, ::_2));
             return true;
         }
         return false;
    }
    
    bool TextReaderNode::canConnectPort(Node &node, std::string sendPort, std::string recPort) {
        return true;
    }
    
    bool TextReaderNode::disconnect(std::string outPort) {
        // only one connection to disconnect
        wire.disconnect();
        return true;
    }
    
    void TextReaderNode::send(std::string sendPort, packet_t output) {
        emitter(sendPort,output);
    }
    
    void TextReaderNode::read(const std::string& fileName) {
        // fix this to send chunks
        mFileName = fileName;
        std::ifstream handle(fileName.c_str(), std::ios_base::in);
        if (!handle.good()) {
            std::cout << "There's a problem with your file." << std::endl;
        }
        // change this to send a line at a time
        std::stringstream buffer;
        buffer << handle.rdbuf();
        // setup and send output packet
        packet_t output = buffer.str();
        // only one port, so it doesn't matter what's in the first argument
        send("STROUT",output);
    }
    
    # pragma mark - StringWriterNode methods
    
    StringWriterNode::StringWriterNode() : Node (), mFileName("NONE") {}
    
    void StringWriterNode::receive(std::string sendPort, packet_t input) {
        // only one non-parameter input, so no question where to send things
        write(input);
    }
    
    void StringWriterNode::write(packet_t input) {
        // process packet
        std::string toWrite;
        fillPacketData(input,toWrite);
        // for now just write to stdout
        std::cout << toWrite << std::endl;
    }
    
    # pragma mark - Tokenizer methods
        
    Tokenizer::Tokenizer() : Node () {}
        
    bool Tokenizer::connectPort(Node &node, std::string sendPort, std::string recPort) {
        if(canConnectPort(node,sendPort,recPort)) {
            wire.disconnect();
            wire = emitter.connect(boost::bind(&Node::receive, &node, ::_1, ::_2));
            return true;
        }
        return false;
    }
    
    bool Tokenizer::canConnectPort(Node &node, std::string sendPort, std::string recPort) {
        return true;
    }
    
    bool Tokenizer::disconnect(std::string outPort) {
        // only one connection to disconnect
        wire.disconnect();
        return true;
    }
    
    void Tokenizer::send(std::string sendPort, packet_t output) {
        emitter(sendPort,output);
    }
    
    void Tokenizer::receive(std::string recPort, packet_t input) {
        // deconstruct packet and send on
        std::string toTokenize;
        fillPacketData(input,toTokenize);
        tokenize(toTokenize);
    }
    
    void Tokenizer::tokenize(std::string& input) {
        boost::char_separator<char> sep(", \t\n");
        boost::tokenizer<boost::char_separator<char>> tokenizer(input,sep);
        for(const auto& t : tokenizer) {
            send("STROUT",std::string(t));
        }
    }
    
}