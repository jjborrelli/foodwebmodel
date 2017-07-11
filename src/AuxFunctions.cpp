/*
 * AuxFunctions.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: manu_
 */

#include "../headers/AuxFunctions.hpp"
using namespace std;

/* A function to split strings according to a given delimiter*/
vector<string> generalSplit(const string& str, const string& delim)
{
    vector<string> tokens;
    int prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

vector<string> split(string &str){
	    return generalSplit(str, string(" "));
}
