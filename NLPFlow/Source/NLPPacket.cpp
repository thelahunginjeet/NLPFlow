//
//  NLPPacket.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 11/8/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPPacket.hpp"

namespace NLP {
    
    Packet::Packet() : mPacketData(0), mStage(Packet::OTHER_PACKET) {}
    
    Packet::Packet(int data, Packet::PACKET_STAGE stage) : mPacketData(data), mStage(stage) {}
    
    Packet::Packet(std::string data, Packet::PACKET_STAGE stage) : mPacketData(data), mStage(stage) {}
    
    bool Packet::isInitial() {
        return (mStage == Packet::INITIAL_PACKET) ? true : false;
    }
    
    bool Packet::isFinal() {
        return (mStage == Packet::FINAL_PACKET) ? true : false;
    }
    
}