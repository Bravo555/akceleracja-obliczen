#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

const std::string delimiter = ",";

std::string readStrFromFile(const std::string &fileName)
{
    std::ifstream f(fileName);
    std::stringstream st;
    st << f.rdbuf();
    return st.str();
}

void writeStrToFile(const std::string &str, const std::string &fileName)
{
    std::ofstream fout(fileName);
    fout << str;
}

void writeResultToFile(const long long elapsedUs, const std::string &resStr, const std::string &fileName)
{
    std::ofstream fout(fileName);

    fout << "elapsed time [us], match indexes\n"
         << elapsedUs
         << delimiter
         << resStr
         << "\n";
}

#endif