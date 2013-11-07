//
//  NLPPort.h
//  NLPFlow
//
//  Created by Kevin Brown on 10/24/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPPort_hpp
#define NLP_NLPPort_hpp

#include <boost/signals2/signal.hpp>
#include <boost/variant.hpp>
#include <queue>

namespace NLP {

    // header-only packet stuff
    typedef boost::variant<std::string,int> packet_t;
    
    // I've deprecated the next two in favor of dropNextPacket.
    // this does the get() but has to be called as get<type>(packet)
    template<typename T>
    T fetchPacketData(packet_t& p) {
        return boost::get<T>(p);
    };
    
    // this one is a void and puts the value into a supplied variable
    template<typename T>
    void fillPacketData(packet_t& p, T& var) {
        var = boost::get<T>(p);
    };
    
    class Port {
    public:
        typedef boost::signals2::signal<void (packet_t)> Emitter;
        typedef boost::signals2::connection Wire;
        
        enum PORT_TYPE {
            TYPE_INT, TYPE_STR, TYPE_NULL
        };
        
        Port(std::string name, PORT_TYPE ptype);
        const std::string name() const;
        const PORT_TYPE type() const;
        void open();
        void close();
        bool isOpen();
    
    protected:
        bool mOpen;
        std::string mName;
        PORT_TYPE mType;
        
    };
    
    class InputPort : public Port {
    public:
        InputPort(std::string name = "IN", PORT_TYPE ptype = TYPE_NULL);
        void receive(packet_t p);
        const bool packetsReady() const;
        
        template<typename T>
        void dropNextPacket(T& var) {
            var = boost::get<T>(mPackets.front());
            mPackets.pop();
        }
        
    private:
        std::queue<packet_t> mPackets;
        
    };
    
    class OutputPort : public Port {
    public:
        OutputPort(std::string name="OUT", PORT_TYPE ptype = TYPE_NULL);
        void send(packet_t p);
        bool connect(InputPort& port);
        bool canConnect(InputPort& port);
        bool disconnect();
        
    private:
        Emitter mEmitter;
        Wire mWire;
    };
    
    
}

#endif
