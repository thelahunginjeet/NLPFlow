//
//  NLPPort.h
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPPort_hpp
#define NLP_NLPPort_hpp

#include "NLPType.hpp"
#include "NLPParameter.hpp"
#include "NLPPacket.hpp"
#include <boost/signals2/signal.hpp>
#include <queue>

namespace NLP {
    
    class Port {
    public:
        typedef boost::signals2::signal<void (Packet)> Emitter;
        typedef boost::signals2::connection Wire;
        
        Port(std::string name, FLOW::PORT_TYPE ptype);
        const std::string name() const;
        const FLOW::PORT_TYPE type() const;
        void open();
        void close();
        virtual bool isOpen();
    
    protected:
        bool mOpen;
        std::string mName;
        FLOW::PORT_TYPE mType;
        
    };
    
    class ParameterPort : public Port {
    public:
        ParameterPort(std::string name="PAR0", FLOW::PORT_TYPE ptype = FLOW::TYPE_NULL);
        void receive(Parameter p);
        Parameter& parameter();
        
    private:
        Parameter mParameter;
    };
    
    class InputPort : public Port {
    public:
        InputPort(std::string name = "IN", FLOW::PORT_TYPE ptype = FLOW::TYPE_NULL);
        void receive(Packet p);
        const bool packetsReady() const;
        virtual bool isOpen();
        void dropNextPacket(Packet& p);
        
    private:
        std::queue<Packet> mPackets;
        
    };
    
    class OutputPort : public Port {
    public:
        OutputPort(std::string name="OUT", FLOW::PORT_TYPE ptype = FLOW::TYPE_NULL);
        void send(Packet p);
        bool connect(InputPort& port);
        bool canConnect(InputPort& port);
        bool disconnect();
        
    private:
        Emitter mEmitter;
        Wire mWire;
    };
    
    
    class ErrorPort : public Port {
    public:
        ErrorPort();
        void send(std::string errMessage);
    };
    
    
    class LogPort : public Port {
    public:
        LogPort();
        void send(std::string logMessage);
    };
    
}

#endif
