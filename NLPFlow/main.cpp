//
//  main.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 10/22/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLP.hpp"
#include <iostream>

int testnetwork() {
    // define the reader and writer
    NLP::TextReaderNode reader;
    NLP::StringWriterNode writer;
    NLP::Tokenizer tokenizer;
    // port names are not paid attention to here
    reader.connectPort(tokenizer,"STROUT","STRIN");
    tokenizer.connectPort(writer,"STROUT","STRIN");
    reader.read("./Tests/smalltest.txt");
    return 0;
}


int testvariant() {
    NLP::packet_t p = 1;
    int j;
    NLP::fillPacketData(p,j);
    std::cout << "Packet contains " << j << std::endl;
    p = "Hello, World!";
    std::string s;
    NLP::fillPacketData(p,s);
    std::cout << "Packet contains " << s << std::endl;
    return 0;
}


int main(int argc, const char * argv[]) {
    testnetwork();
    return 0;
}


