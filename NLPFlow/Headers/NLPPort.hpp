//
//  NLPPort.h
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPPort_hpp
#define NLP_NLPPort_hpp

#include "NLPPacket.hpp"
#include <boost/signals2/signal.hpp>
#include <queue>

namespace NLP {
    
    class Port {
    public:
        typedef boost::signals2::signal<void (Packet)> Emitter;
        typedef boost::signals2::connection Wire;
        
        enum PORT_TYPE {
            TYPE_INT, TYPE_STR, TYPE_NULL
        };
        
        Port(std::string name, PORT_TYPE ptype);
        const std::string name() const;
        const PORT_TYPE type() const;
        void open();
        void close();
        virtual bool isOpen();
    
    protected:
        bool mOpen;
        std::string mName;
        PORT_TYPE mType;
        
    };
    
    class ParameterPort : public Port {
    public:
        ParameterPort(std::string name="PARAMETER", PORT_TYPE ptype = TYPE_NULL);
        void receive(Packet p);
        Packet& parameter();
        
    private:
        Packet mParameterPacket;
    };
    
    class InputPort : public Port {
    public:
        InputPort(std::string name = "IN", PORT_TYPE ptype = TYPE_NULL);
        void receive(Packet p);
        const bool packetsReady() const;
        virtual bool isOpen();
        void dropNextPacket(Packet& p);
        
        //template<typename T>
        //void dropNextPacket(T& var) {
        //    var = boost::get<T>(mPackets.front());
        //    mPackets.pop();
        //}
        
    private:
        std::queue<Packet> mPackets;
        
    };
    
    class OutputPort : public Port {
    public:
        OutputPort(std::string name="OUT", PORT_TYPE ptype = TYPE_NULL);
        void send(Packet p);
        bool connect(InputPort& port);
        bool canConnect(InputPort& port);
        bool disconnect();
        
    private:
        Emitter mEmitter;
        Wire mWire;
    };    
    
}

#endif
