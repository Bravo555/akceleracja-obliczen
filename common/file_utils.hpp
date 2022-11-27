#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

const std::string fieldDelimiter = ", ";
std::string indexDelimiter = ";";

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

std::string matchesVectorToStr(const std::vector<int> &matches)
{
    std::string res = "";
    for (int match : matches)
    {
        res += std::to_string(match);
        res += indexDelimiter;
    }
    return res;
}

void writeCsvHeader(const std::string &fileName)
{
    std::ofstream fout(fileName);
    fout << "elapsed time [us], number of matches, matches\n";
}

void appendResultToFile(const long long elapsedUs, const std::vector<int> &matches, const std::string &fileName)
{
    std::ofstream fout(fileName, std::ios_base::app);
    std::string matchesStr = matchesVectorToStr(matches);

    fout << elapsedUs
         << fieldDelimiter
         << matches.size()
         << fieldDelimiter
         << matchesStr
         << "\n";
}

void writeResultToFile(const long long elapsedUs, const std::vector<int> &matches, const std::string &fileName)
{
    writeCsvHeader(fileName);
    appendResultToFile(elapsedUs, matches, fileName);
}

#endif