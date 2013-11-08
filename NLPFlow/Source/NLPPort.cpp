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
    
    Port::Port(std::string name, PORT_TYPE ptype) : mName(name), mType(ptype), mOpen(false) {}
    
    const std::string Port::name() const {
        return mName;
    }
    
    const Port::PORT_TYPE Port::type() const {
        return mType;
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
    
# pragma mark - ParameterPort methods
    ParameterPort::ParameterPort(std::string name, PORT_TYPE ptype) : Port(name, ptype) {}
    
    void ParameterPort::receive(Packet p) {
        // hold the parameter and open
        mParameterPacket = p;
        mOpen = true;
        return;
    }
    
    Packet& ParameterPort::parameter() {
        return mParameterPacket;
    }
    
# pragma mark - InputPort methods
    
    InputPort::InputPort(std::string name, PORT_TYPE ptype) : Port(name, ptype) {}
    
    void InputPort::receive(Packet p) {
        // put the packet into the queue
        mPackets.push(p);
        return;
    }
    
    bool InputPort::isOpen() {
        return (mPackets.size() > 0) ? true : false;
    }
    
    void InputPort::dropNextPacket(Packet& p) {
        p = mPackets.front();
        mPackets.pop();
    }
    
# pragma mark - OutputPort methods
    
    OutputPort::OutputPort(std::string name, PORT_TYPE ptype) : Port(name, ptype) {}
        
    void OutputPort::send(Packet p) {
        // send the packet
        mEmitter(p);
        return;
    }
    
    bool OutputPort::canConnect(InputPort& port) {
        // checks compatible type AND insures type has been set for port
        PORT_TYPE oType = type();
        PORT_TYPE iType = port.type();
        if((oType == iType) && (oType != TYPE_NULL) && (iType != TYPE_NULL)) {
            return true;
        }
        return false;
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