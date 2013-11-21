//
//  NLPParameter.h
//  NLPFlow
//
//  Created by Kevin Brown on 11/18/13.
//  Copyright (c) 2013 Kevin Brown. All rights reserved.
//

#ifndef NLP_NLPParameter_hpp
#define NLP_NLPParameter_hpp

#include <iostream>
#include <boost/variant.hpp>

namespace NLP {
    
    class Parameter {
    public:
        typedef boost::variant<int,std::string> parameter_t;
        Parameter() : mParameterData(0) {};
        Parameter(int data) : mParameterData(data) {};
        Parameter(std::string data) : mParameterData(data) {};
        
        // this does the get() but has to be called as get<type>(parameter)
        template<typename T>
        T fetchParameterData() {
            return boost::get<T>(mParameterData);
        };
        
        // this one is a void and puts the value into a supplied variable
        template<typename T>
        void fillParameterData(T& var) {
            var = boost::get<T>(mParameterData);
        };
        
    private:
        parameter_t mParameterData;
    };
    
}


#endif /* defined(NLP_NLPParameter_hpp) */
