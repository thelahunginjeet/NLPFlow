//
//  NLPPort.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPPort.hpp"

namespace NLP {
    
# pragma mark - Port methods
    
    Port::Port(std::string name) : mName(name), mOpen(false) {}
    
    const std::string Port::name() const {
        return mName;
    }
    
    void Port::open() {
        mOpen = true;
    }
    
    void Port::close() {
        mOpen = false;
    }
    
    bool Port::isOpen() {
        return mOpen;
    }
    
# pragma mark - InputPort methods
    
    InputPort::InputPort(std::string name) : Port(name) {}
    
    void InputPort::receive(packet_t input) {
        // store the recieved packet and open the port for later access
        mCurrentPacket = input;
        mOpen = true;
        return;
    }
    
# pragma mark - OutputPort methods
    
    OutputPort::OutputPort(std::string name) : Port(name) {}
    
    void OutputPort::send(packet_t output) {
        // send the packet and close the port after send
        mEmitter(output);
        mOpen  = false;
        return;
    }
    
    bool OutputPort::canConnect(InputPort& port) {
        return true;
    }
    
    bool OutputPort::connect(InputPort& port) {
        if(canConnect(port)) {
            mWire.disconnect();
            mWire = mEmitter.connect(boost::bind(&InputPort::receive, &port, ::_1));
            return true;
        }
        return false;
    }
    
    bool OutputPort::disconnect() {
        mWire.disconnect();
        return true;
    }
    
}