//
//  main.cpp
//  DES
//
//  Created by 张家豪 on 2018/12/7.
//  Copyright © 2018 张家豪. All rights reserved.
//

#include <iostream>
#include "DES.h"
#include "DEStable.h"
#include <fstream>
#include <vector>

bits hex2byte(string input){
    bits res;
    cout << input << endl;
    for(int i = 0, j = 3; i < 64; i++){
        int tmp = input[i/4];
        if(tmp <= '9' && tmp >= '0')
            tmp -= '0';
        else
            tmp -= 'a' - 10;
        res.push_back( (tmp >> j) & 1);
        j--;
        if(j == -1)
            j = 3;
    }
    return res;
}

vector<bits> hex2bytes(string input){
    vector<bits> res;
    for(int i = 0,size = input.size(); i < size; i = i+16){
        string tmp = input.substr(i,16);
        res.push_back(hex2byte(tmp));
    }
    return res;
}

string byte2hex(bits input){
    string res = "";
    for(int i = 0,size = input.size(); i < size; i=i+4){
        char tmp = 0;
        for(int j = 0; j < 4;j++){
            tmp |= int(input[i+j]) << (3-j);
        }
        if(tmp < 10)
            res += tmp +'0';
        else
            res += (tmp-10) + 'a';
    }
    return res;
}

string bytes2hex(vector<bits> input){
    string res;
    for(int i = 0,size = input.size(); i < size; i++){
        res += byte2hex(input[i]);
    }
    return res;
}

int main(int argc, const char * argv[]) {
    string k = "张家豪";
    bits key = genKey(k);
    vector<bits> M = string2bytes(k);
    cout << M << endl << endl;
    cout << bytes2hex(M) << endl << endl;
    cout << hex2bytes(bytes2hex(M));
    
    vector<bits> C = doEncrypt(M, key);
//    cout << bytes2string(C) << endl;
    
//    string myString ="阿迪算法 i 哦啊大煞风景";
//    std::vector<char> bytes(myString.begin(), myString.end());
//    bytes.push_back('\0');
//    char *c = &bytes[0];
//    cout << c;
//
//    vector<bits> M_ = doDecrypt(C, key);
//    cout << bytes2string(M_) << endl;

    return 0;
}

