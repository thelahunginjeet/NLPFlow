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

namespace NLP {

    // header-only packet stuff
    typedef boost::variant<std::string,int> packet_t;
    
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
        
        Port(std::string name);
        const std::string name() const;
        void open();
        void close();
        bool isOpen();
    
    protected:
        bool mOpen;
        std::string mName;
        
    };
    
    class InputPort : public Port {
    public:
        InputPort(std::string name = "IN");
        void receive(packet_t input);
    
    private:
        packet_t mCurrentPacket;
        
    };
    
    class OutputPort : public Port {
    public:
        OutputPort(std::string name="OUT");
        void send(packet_t output);
        bool connect(InputPort& port);
        bool canConnect(InputPort& port);
        bool disconnect();
        
    private:
        Emitter mEmitter;
        Wire mWire;
    };
    
    
}

#endif
