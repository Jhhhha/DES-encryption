//
//  DES.h
//  DES
//
//  Created by 张家豪 on 2018/12/7.
//  Copyright © 2018 张家豪. All rights reserved.
//

#ifndef DES_H
#define DES_H
#include <string>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include "DEStable.h"

using namespace std;
typedef vector<bool> bits;


bits charToByte(const char *In, int bit_count) {
    bits Out;
    for (int i = 0; i < bit_count; i++)
        Out.push_back( (In[i / 8] >> (i % 8)) & 1);
    return Out;
}

void charToByte(bits Out, const char *In, int bit_count) {
    for (int i = 0; i < bit_count; i++)
        Out[i] = (In[i / 8] >> (i % 8)) & 1;
}

//从bool数到char数组

void byteToChar(char *Out, const bool *In, int bits) {
    memset(Out, 0, (bits + 7) / 8);
    for (int i = 0; i < bits; i++)
    {
        Out[i / 8] |= In[i] << (i % 8);
    }
}


ostream & operator<<(ostream &out, vector<bool> tmp) {
    for(int i = 0, size = tmp.size(); i < size; i++){
        out << tmp[i] << " ";
        if((i+1) % 8 == 0)
            out << endl;
    }
    return out;
}

ostream & operator<<(ostream &out, vector<bits> tmp) {
    for(int i = 0, size = tmp.size(); i < size; i++)
        out << tmp[i] << endl;
    return out;
}

bits operator ^ (const bits &tmp, const bits & p){//操作必须位数相同
    bits res;
    for(int i = 0, size = tmp.size(); i < size; i++)
        res.push_back(tmp[i] ^ p[i]);
    return res;
}

bits operator>>(const bits& tmp, int shift){
    vector<bool> res(tmp);
    for(int i = 0; i < shift; i++){
        bool t = res.back();
        res.pop_back();
        res.insert(res.begin(), t);
    }
    return res;
}

bits operator<<(const bits& tmp, int shift){
    vector<bool> res(tmp);
    for(int i = 0; i < shift; i++){
        bool t = res.front();
        res.erase(res.begin());
        res.push_back(t);
    }
    return res;
}

bits char2byte(const string in, int bit){
    vector<bool> res;
    string tmp = in + "        "; //至少8位
    for(int i = 0; i < bit; i++){
        //        int p = tmp[i / 8];
        //        cout << p << ":" << ((tmp[i / 8] >> (i % 8)) & 1) << endl;
        res.push_back( (tmp[i / 8] >> (i % 8)) & 1);
    }
    return res;
}

bits bits_merge(const bits& left, const bits& right){
    //    cout << "left:" << endl << left << endl;
    bits res = left;
    //    cout << res << endl;
    for(int i = 0, size = right.size(); i < size; i++)
        res.push_back(right[i]);
    return res;
}

bits Extend(const int *extendTable, bits input){ //32位扩展
    bits res;
    for(int i = 0, size = input.size(); i < size; i++){
        res.push_back(input[extendTable[i]-1]);
    }
    return res;
}

bits Pbox_f(const int *IPTable, bits input, int bit_count){ //
    bits res;
    for(int i = 0; i < bit_count; i++){
        res.push_back(input[IPTable[i]-1]);
    }
    return res;
}

bits Sbox_f(const bits &input){//48位输入，32位输出
    bits res;
    for(int i = 0; i < 8; i=i+6){
        int row = (input[i] << 1) + (input[i+5]);
        int column = (input[i+1] << 3) + (input[i+2] << 2) + (input[i+3] << 1) + input[i+4];
        int tmp = SBox[i][row][column];
        for(int i = 0 ; i < 4; i++){
            res.insert(res.begin(), (tmp & 1));
            tmp >>= 1;
        }
    }
    return res;
}

bits circle_fun(bits input, bits key) {//圈函数
    bits Extended = Extend(extendTable, input); //E盒拓展变48位
    Extended = Extended ^ key;  //与48位圈密钥异或
    bits res = Sbox_f(Extended);    //S盒压缩为32位
    return Pbox_f(PTable, res, 32); //P盒置换，返回32位结果
}

bits encrypt(bits input_, bits key, int round_time){
    bits res;
    //    cout << "明文输入：" << endl << input_ << endl;
    bits input = Pbox_f(initIPTable, input_, 64);
    //    cout << "初始置换：" << endl << input << endl;
    
    auto mid = input.begin();
    for(int i = 0; i < 32; i++, mid++);
    bits left = bits(input.begin(), mid);
    bits right = bits(mid, input.end());
    //    cout << "L0: " << endl << left << endl << "R0:" << endl << right << endl;
    
    bits key56 = Pbox_f(PC_1Table, key, 56);
    mid = key56.begin();
    for(int i = 0; i < 28; i++, mid++);
    bits key_left = bits(key56.begin(), mid);
    bits key_right = bits(mid, key56.end());
    bits key_tmp1 = bits_merge(key_left, key_right);
    
    for(int i = 0; i < round_time; i++){
        key_left = key_left << (shiftTable[i]);
        key_right = key_right << (shiftTable[i]);
        bits key_tmp = bits_merge(key_left, key_right);
        bits key48 = Pbox_f(PC_2Table, key_tmp, 48);
        
        bits tmp = left;
        left = right;
        right = circle_fun(right, key48) ^ tmp;
        //        cout << "M" << i+1 << ":" << endl << bits_merge(left, right) << endl;
        //        cout << "L" << i+1 << ": " << endl << left << endl << "R" << i+1 << ":" << endl << right << endl;
    }
    res = bits_merge(right, left);
    res = Pbox_f(inverseIPTable, res, 64);
    //    cout << "逆初始置换:" << endl << res << endl;
    return res;
}

bits decrypt(bits input_, bits key, int round_time){
    bits res;
    //    cout << "密文输入：" << endl << input_ << endl;
    bits input = Pbox_f(initIPTable, input_, 64); //初始置换
    //    cout << "初始置换：" << endl << input << endl;
    
    auto mid = input.begin();
    for(int i = 0; i < 32; i++, mid++);
    bits left = bits(input.begin(), mid);
    bits right = bits(mid, input.end());
    //    cout << "L0: " << endl << left << endl << "R0:" << endl << right << endl;
    
    bits key56 = Pbox_f(PC_1Table, key, 56);
    mid = key56.begin();
    for(int i = 0; i < 28; i++, mid++);
    bits key_left = bits(key56.begin(), mid);
    bits key_right = bits(mid, key56.end());
    key_left = key_left << 29;
    key_right = key_right << 29;
    
    bits key_tmp1 = bits_merge(key_left, key_right);
    
    for(int i = 0; i < round_time; i++){
        key_left = key_left >> (shiftTable[(16-i)%16]);
        key_right = key_right >> (shiftTable[(16-i)%16]);
        bits key_tmp = bits_merge(key_left, key_right);
        bits key48 = Pbox_f(PC_2Table, key_tmp, 48);
        
        bits tmp = left;
        left = right;
        right = circle_fun(right, key48) ^ tmp;
        //        cout << "M" << 16-i << ":" << endl<< bits_merge(left, right) << endl;
        //        cout << "L" << i+1 << ": " << endl << left << endl << "R" << i+1 << ":" << endl << right << endl;
    }
    res = bits_merge(right, left);
    res = Pbox_f(inverseIPTable, res, 64); //初始置换逆置换
    //    cout << "逆初始置换:" << endl << res << endl;
    return res;
}

bits genKey(string key){
    string tmp = key + "        ";//补全不足64位的密钥
    string realKey = tmp.substr(0, 8);
    return char2byte(realKey, 64);
}

vector<bits> string2bytes(string Message){
    vector<bits> res;
    int i = 0;
    for(int size = Message.size(); i + 7 < size; i = i + 8){
        res.push_back(char2byte(Message.substr(i, i+8), 64));
    }
    if((Message.size() % 8) == 0)
        return res;
    else{
        string last = Message.substr(i);
        char tmp = 0;
        while(last.size() != 8)
            last += tmp;
        res.push_back(char2byte(last, 64));
    }
    return res;
}

string b2s(bits Cipher, int bits_count){
    int tmp[8] = {0};
    string res;
    for (int i = 0; i < bits_count; i++){
        int temp = Cipher[i];
        tmp[i / 8] |= temp << (i % 8);
        //        cout << tmp[i/8] << endl;
    }
    for(int i = 0; i < 8; i++)
        res.push_back(tmp[i]);
    return res;
}

string bytes2string(vector<bits> Cipher){
    string res;
    for(int i = 0,size = Cipher.size(); i < size; i++){
        res += b2s(Cipher[i], 64);
    }
    return res;
}

vector<bits> doEncrypt(vector<bits> Message, bits key){
    vector<bits> res;
    for(int i = 0, size = Message.size(); i < size; i++)
        res.push_back(encrypt(Message[i], key, 16));
    return res;
}

vector<bits> doDecrypt(vector<bits> C, bits key){
    vector<bits> res;
    for(int i = 0, size = C.size(); i < size; i++)
        res.push_back(decrypt(C[i], key, 16) );
    return res;
}


#endif /* DES_h */
