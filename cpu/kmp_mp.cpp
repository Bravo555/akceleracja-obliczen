
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include "../common/file_utils.hpp"
#include "../common/timer.hpp"

std::vector<int> buildPartialMatchTable(const std::string &soughtWord)
{
    std::vector<int> partialMatchTable(soughtWord.length() + 1);
    partialMatchTable.at(0) = -1;
    int currPos = 1;
    int indexOfNextChar = 0;

    while (currPos < soughtWord.length())
    {
        if (soughtWord[currPos] == soughtWord[indexOfNextChar])
        {
            partialMatchTable[currPos] = partialMatchTable[indexOfNextChar];
        }
        else
        {
            partialMatchTable[currPos] = indexOfNextChar;
            while (indexOfNextChar >= 0 &&
                   soughtWord[currPos] != soughtWord[indexOfNextChar])
            {
                indexOfNextChar = partialMatchTable[indexOfNextChar];
            }
        }
        currPos++;
        indexOfNextChar++;
    }
    partialMatchTable[currPos] = indexOfNextChar;

    return partialMatchTable;
}

std::vector<int> searchWord(const std::string &text, const std::string &soughtWord, int offset = 0)
{
    int soughtWordLen = soughtWord.length();
    std::vector<int> foundIndexVector = {};
    int currPosInText = 0;
    int currPosInWord = 0;
    std::vector<int> partialMatchTable = buildPartialMatchTable(soughtWord);

    while (currPosInText < text.length())
    {
        if (soughtWord[currPosInWord] == text[currPosInText])
        {
            currPosInText++;
            currPosInWord++;
            if (currPosInWord == soughtWordLen && partialMatchTable.at(soughtWordLen) != -1)
            {
                foundIndexVector.push_back(currPosInText - currPosInWord + offset);
                currPosInWord = partialMatchTable.at(currPosInWord);
            }
        }
        else
        {
            currPosInWord = partialMatchTable.at(currPosInWord);
            if (currPosInWord < 0)
            {
                currPosInText++;
                currPosInWord++;
            }
        }
    }

    return foundIndexVector;
}

std::pair<std::vector<std::string>, std::vector<int>> divideTextWithOffsets(const std::string &text, int numberOfSections)
{
    std::vector<std::string> dividedText = std::vector<std::string>();
    std::vector<int> offsets = std::vector<int>();
    int sectionLen;
    if (numberOfSections == 0 || numberOfSections == 1)
    {
        sectionLen = text.length();
        dividedText.push_back(text);
        return std::pair<std::vector<std::string>, std::vector<int>>(dividedText, offsets);
    }
    else
    {
        sectionLen = text.length() / numberOfSections;
    }
    int lastDivisionIndex = 0;

    for (int i = 1; i < numberOfSections + 1; i++)
    {
        if (sectionLen * i >= text.length())
        {
            printf("Something went wrong!");
            continue;
        }
        int length = text.find_first_of(" ", sectionLen * i) - lastDivisionIndex;
        std::string temp = text.substr(lastDivisionIndex, length);
        offsets.push_back(lastDivisionIndex);
        lastDivisionIndex += length;
        dividedText.push_back(temp);
    }

    return std::pair<std::vector<std::string>, std::vector<int>>(dividedText, offsets);
}

std::vector<int> multiThreadKMP(const std::string &text, const std::string &keyword)
{
    std::vector<int> results = std::vector<int>();
    std::pair<std::vector<std::string>, std::vector<int>> dividedTextAndOffsets = divideTextWithOffsets(text, omp_get_max_threads());
#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < omp_get_num_threads(); i++)
        {
            std::vector<int> tempResults = searchWord(dividedTextAndOffsets.first.at(i), keyword, dividedTextAndOffsets.second.at(i));

#pragma omp critical
            results.insert(results.end(), tempResults.begin(), tempResults.end());
        }
    }
    return results;
}

int main(int argc, char *argv[])
{
    std::string textFile = argv[1];
    std::string keywordFile = argv[2];
    std::string resultsFile = argv[3];
    Timer timer;

    std::string text = readStrFromFile(textFile);
    std::string keyword = readStrFromFile(keywordFile);

    timer.start();
    std::vector<int> results = multiThreadKMP(text, keyword);
    long long elapsedUs = timer.getElapsedMicroseconds();
    writeResultToFile(elapsedUs, results, resultsFile);

    return 0;
}