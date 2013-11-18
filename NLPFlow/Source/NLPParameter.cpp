//
//  NLPParameter.cpp
//  NLPFlow
//
//  Created by Kevin Brown on 11/18/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#include "NLPParameter.hpp"


namespace NLP {
    
    Parameter::Parameter() : mParameterData(0) {}
    
    Parameter::Parameter(int data) : mParameterData(data) {}
    
    Parameter::Parameter(std::string data) : mParameterData(data) {}
}