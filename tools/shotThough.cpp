/*
Copyright (c) 2015, Wator Vapor
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of wator nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <iostream>
#include <vector>
using namespace std;


const uint64_t iuConstMinNum25 =0b0000000000000000000011111;
const uint64_t iuConstMaxNum25 =0b1111100000000000000000000 + 1;

//#define LOG_D


int isON(int x,int y,vector<vector<bool>> &rows) {
#ifdef LOG_D
    cout << x << "," << y << endl;
#endif
    if (x>=rows[0].size() || x < 0)
    {
        return 0;
    }
    if (y>=rows.size() || y < 0)
    {
        return 0;
    }
#ifdef LOG_D
    cout << x << "," << y << ":rows[x][y]" << rows[y][x] << endl;
#endif
    if(rows[y][x])
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

bool isShotThough(std::bitset<25> bit){
    if(bit.count() != 5) {
        return false;
    }
    vector<vector<bool>>rows;
    for(int y = 0;y< 5 ;y++) {
        vector<bool> row;
        for(int x = 0;x< 5 ;x++) {
            int index = y * 5 + x;
            if(bit[index]){
                row.push_back(true);
            } else {
                row.push_back(false);
            }
        }
        rows.push_back(row);
    }
    
#ifdef LOG_D
    cout << bit <<endl;
#endif
    for(int y = 0;y< 5 ;y++) {
        for(int x = 0;x< 5 ;x++) {
            if(rows[y][x]){
                int lX = x -1;
                int rX = x +1;
                int tY = y-1;
                int bY = y +1;
                bool sum =0;
                sum += isON(lX,tY,rows);
                sum += isON(x,tY,rows);
                sum += isON(rX,tY,rows);
                sum += isON(lX,y,rows);
                sum += isON(rX,y,rows);
                sum += isON(lX,bY,rows);
                sum += isON(x,bY,rows);
                sum += isON(rX,bY,rows);
#ifdef LOG_D
                cout << x << "," << y << ":   " << sum <<endl;
#endif
                if(sum == 0) {
                    return false;
                }
            }
        }
    }
    bool rowSpace = false;
    for(int y = 0;y< 5 ;y++) {
        int sumRow = 0;
        for(int x = 0;x< 5 ;x++) {
            if(rows[y][x]) {
                sumRow++;
            }
        }
        if(0 ==sumRow){
            rowSpace = true;
            break;
        }
    }

    bool colSpace = false;
    for(int x = 0;x< 5 ;x++) {
        int sumCol = 0;
        for(int y = 0;y< 5 ;y++) {
            if(rows[y][x]) {
                sumCol++;
            }
        }
        if(0 ==sumCol){
            colSpace = true;
            break;
        }
    }
#ifdef LOG_D
    cout << bit <<endl;
    cout << rowSpace << colSpace <<endl;
#endif
    if(rowSpace && colSpace) {
        return false;
    }

#if 0
    int sumRow0 = 0;
    int sumRow4 = 0;
    for(int x = 0;x< 5 ;x++) {
        sumRow0 += rows[0][x];
        sumRow4 += rows[4][x];
    }
    int sumCol0 = 0;
    int sumCol4 = 0;
    for(int y = 0;y< 5 ;y++) {
        sumCol0 += rows[y][0];
        sumCol4 += rows[y][4];
    }
    if(0==sumRow0 && 0==sumRow4 && 0==sumCol0 && 0==sumCol4){
        return false;
    }
#endif
    
    
    
    return true;
}

/*
 
11110
00000
10000
00000
00000
 
10000
10011
00100
00000
00000
 
00000
10010
01100
00010
00000
 
00000
01100
00011
00100
00000
 
11110
00001
00000
00000
00000

 
11100
10100
00000
00000
00000
 
11000
00100
00011
00000
00000
 
00011
01000
10100
00000
00000

00011
10000
01000
00100
00000
 
 */

int main(int ac,char *av[]){
    int counter = 0;
    for(uint64_t i = iuConstMinNum25;i < iuConstMaxNum25 ;i++)
    {
        std::bitset<25> memBit(i);
        if(isShotThough(memBit)) {
            cout<< "{0b" << memBit << ",true},"<<endl;
            counter++;
        }
    }
    cout<< counter <<endl;
    return 0;
}
