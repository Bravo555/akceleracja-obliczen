
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "../common/file_utils.hpp"
#include "../common/timer.hpp"

std::vector<int> buildPartialMatchTable(const std::string &soughtWord)
{
    std::vector<int> partialMatchTable(soughtWord.length() + 1);
    partialMatchTable.at(0) = -1;
    size_t currPos = 1;
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

std::vector<int> searchWord(const std::string &text, const std::string &soughtWord, int offset, int limit)
{
    int soughtWordLen = soughtWord.length();
    std::vector<int> foundIndexVector = {};
    int currPosInText = offset;
    int currPosInWord = 0;
    std::vector<int> partialMatchTable = buildPartialMatchTable(soughtWord);

    while (currPosInText < limit)
    {
        if (soughtWord[currPosInWord] == text[currPosInText])
        {
            currPosInText++;
            currPosInWord++;
            if (currPosInWord == soughtWordLen && partialMatchTable.at(soughtWordLen) != -1)
            {
                foundIndexVector.push_back(currPosInText - currPosInWord);
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

std::vector<int> getTextOffsets(const std::string &text, int numberOfSections)
{
    std::vector<int> offsets = std::vector<int>();
    int sectionLen;
    if (numberOfSections == 0 || numberOfSections == 1)
    {
        offsets.push_back(0);
        offsets.push_back(text.length());
        return offsets;
    }
    else
    {
        sectionLen = text.size() / numberOfSections;
    }
    int lastDivisionIndex = 0;
    for (int i = 1; i < numberOfSections + 1; i++)
    {
        int length = text.find_first_of(" ", sectionLen * i) - lastDivisionIndex;
        offsets.push_back(lastDivisionIndex);
        lastDivisionIndex += length;
    }
    offsets.push_back(text.length());

    return offsets;
}

std::vector<int> multiThreadKMP(const std::string &text, const std::string &keyword)
{
    std::vector<int> results = std::vector<int>();
    std::vector<int> textOffsets = getTextOffsets(text, omp_get_max_threads());
#pragma omp parallel
    {
#pragma omp for
        for (int i = 0; i < omp_get_max_threads(); i++)
        {
            std::vector<int> tempResults = searchWord(text, keyword, textOffsets.at(i), textOffsets.at(i + 1));

#pragma omp critical
            results.insert(results.end(), tempResults.begin(), tempResults.end());
        }
    }
    return results;
}

std::vector<int> singleThreadKMP(const std::string &text, const std::string &keyword)
{
    return searchWord(text, keyword, 0, text.length());
}