//
//  RotaryEncoder.cpp
//  serialTest
//
//  Created by JeongHo Park on 15.03.18.
//

#include "RotaryEncoder.hpp"


void RotaryEncoder::setup(string _address){
    serial.listDevices();
    serial.setup(_address, 19200);

    rotA = 0;
    rotB = 0;
    rotC = 0;
    rotD = 0;

    font.load("verdana.ttf", 100, true, false, true, 0.4, 28);
}



void RotaryEncoder::update(){
    unsigned char tmp[20] = {};
    vector <char> numA;
    vector <char> numB;
    vector <char> numC;
    vector <char> numD;

    bool isStartA = false;
    bool isEndA = false;
    bool isStartB = false;
    bool isEndB = false;
    bool isStartC = false;
    bool isEndC = false;
    bool isStartD = false;
    bool isEndD = false;

    if(serial.available()){
        serial.readBytes(tmp, 20);

        for(int i=0; i<20; i++){
            if(tmp[i] == 'A'){
                isStartA = true;
            }else if(tmp[i] == 'a'){
                isEndA = true;
            }else{
                if(isStartA && !isEndA){
                    if(i == 19)
                        numA.clear();
                    else
                        numA.push_back(tmp[i]);
                }
            }

            if(tmp[i] == 'B'){
                isStartB = true;
            }else if(tmp[i] == 'b'){
                isEndB = true;
            }else{
                if(isStartB && !isEndB){
                    if(i == 19)
                        numB.clear();
                    else
                        numB.push_back(tmp[i]);
                }
            }
            if(tmp[i] == 'C'){
                isStartC = true;
            }else if(tmp[i] == 'c'){
                isEndC = true;
            }else{
                if(isStartC && !isEndC){
                    if(i == 19)
                        numC.clear();
                    else
                        numC.push_back(tmp[i]);
                }
            }
            if(tmp[i] == 'D'){
                isStartD = true;
            }else if(tmp[i] == 'd'){
                isEndD = true;
            }else{
                if(isStartD && !isEndD){
                    if(i == 19)
                        numD.clear();
                    else
                        numD.push_back(tmp[i]);
                }
            }
        }

        isStartA = false;
        isEndA = false;
        isStartB = false;
        isEndB = false;
        isStartC = false;
        isEndC = false;
        isStartD = false;
        isEndD = false;

        if(numA.size() > 0){
            rotA = vtoi(numA, numA.size()-1) + 126;
            //            cout << "A " << vtoi(numA, numA.size()-1) << endl;
        }
        if(numB.size() > 0){
            rotB = vtoi(numB, numB.size()-1) + 126;
            //            cout << "B " << vtoi(numB, numB.size()-1) << endl;
        }
        if(numC.size() > 0){
            rotC = vtoi(numC, numC.size()-1) + 126;
            //            cout << "C " << vtoi(numC, numC.size()-1) << endl;
        }
        if(numD.size() > 0){
            rotD = vtoi(numD, numD.size()-1) + 126;
            //            cout << "D " << vtoi(numD, numD.size()-1) << endl;
        }
    }

    numA.clear();
    numB.clear();
    numC.clear();
    numD.clear();
    serial.flush();
}


int RotaryEncoder::vtoi(vector<char> vec, int end) {
    int minplus = 1;
    int beg = 0;

    if (vec[0] == '-') {
        minplus = -1;
        beg = 1;
    } else {
        minplus = 1;
        beg = 0;
    }

    int ret = 0;
    int mult = pow(10 , (end-beg));

    for(int i = beg; i <= end; i++) {
        ret += (vec[i] - '0') * mult;
        mult /= 10;
    }

    return ret * minplus;
}


