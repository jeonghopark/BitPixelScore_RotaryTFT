//
//  RotaryEncoder.hpp
//  serialTest
//
//  Created by JeongHo Park on 15.03.18.
//

#ifndef RotaryEncoder_hpp
#define RotaryEncoder_hpp

#include "ofMain.h"

class RotaryEncoder {


public:
    void setup(string _address);
    void update();
    void inputData();

    int vtoi(vector<char> vec, int end);

    ofSerial serial;

    int rotA, rotB, rotC, rotD;
    ofTrueTypeFont font;

};


#endif /* RotaryEncoder_hpp */
