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
    
    Port::Port(std::string name, FLOW::PORT_TYPE ptype) : mName(name), mType(ptype), mOpen(false) {}
    
    const std::string Port::name() const {
        return mName;
    }
    
    const FLOW::PORT_TYPE Port::type() const {
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
    ParameterPort::ParameterPort(std::string name, FLOW::PORT_TYPE ptype) : Port(name, ptype) {}
    
    void ParameterPort::receive(Parameter p) {
        // hold the parameter and open
        mParameter = p;
        mOpen = true;
        return;
    }
    
    Parameter& ParameterPort::parameter() {
        return mParameter;
    }
    
# pragma mark - InputPort methods
    
    InputPort::InputPort(std::string name, FLOW::PORT_TYPE ptype) : Port(name, ptype) {}
    
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
    
    OutputPort::OutputPort(std::string name, FLOW::PORT_TYPE ptype) : Port(name, ptype) {}
        
    void OutputPort::send(Packet p) {
        // send the packet
        mEmitter(p);
        return;
    }
    
    bool OutputPort::canConnect(InputPort& port) {
        // checks compatible type AND insures type has been set for port
        // an input port of TYPE_ANY can connect to any output port
        FLOW::PORT_TYPE oType = type();
        FLOW::PORT_TYPE iType = port.type();
        // type check
        if((oType == iType) && (oType != FLOW::TYPE_NULL) && (iType != FLOW::TYPE_NULL)) {
            return true;
        }
        if((oType != FLOW::TYPE_NULL) && (iType == FLOW::TYPE_ANY)){
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

# pragma mark - ErrorPort methods
    
    ErrorPort::ErrorPort() : Port("ERROR",FLOW::TYPE_STR) {}
    
    void ErrorPort::send(std::string errMessage) {
        std::cerr << errMessage;
        return;
    }
    
# pragma mark - LogPort methods
    
    LogPort::LogPort() : Port("LOG",FLOW::TYPE_STR) {}
    
    void LogPort::send(std::string logMessage) {
        std::cout << logMessage;
        return;
    }
    
}
