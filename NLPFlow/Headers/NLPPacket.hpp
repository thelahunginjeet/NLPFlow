//
//  NLPPacket.h
//  NLPFlow
//
//  Created by Kevin Brown on 11/8/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPPacket_hpp
#define NLP_NLPPacket_hpp

#include <iostream>
#include <boost/variant.hpp>

namespace NLP {

    class Packet {
    public:
        typedef boost::variant<int,std::string> packet_t;
        enum PACKET_STAGE {
            INITIAL_PACKET, FINAL_PACKET, OTHER_PACKET
        };
        
        Packet();
        Packet(int data, PACKET_STAGE stage = OTHER_PACKET);
        Packet(std::string data, PACKET_STAGE stage = OTHER_PACKET);
        bool isInitial();
        bool isFinal();
        
        // this does the get() but has to be called as get<type>(packet)
        template<typename T>
        T fetchPacketData() {
            return boost::get<T>(mPacketData);
        };
    
        // this one is a void and puts the value into a supplied variable
        template<typename T>
        void fillPacketData(T& var) {
            var = boost::get<T>(mPacketData);
        };
    
    private:
        packet_t mPacketData;
        PACKET_STAGE mStage;
    };
    
}


#endif /* defined(__NLPFlow__NLPPacket__) */
