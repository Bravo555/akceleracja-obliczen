#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>

std::vector<int> buildPartialMatchTable(std::string soughtWord)
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

std::pair<std::vector<int>, int> searchWord(std::string text, std::string soughtWord)
{
    int soughtWordLen = soughtWord.length();
    std::vector<int> foundIndexVector = {};
    int numberOfWords = 0;
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
                foundIndexVector.push_back(currPosInText - currPosInWord);
                numberOfWords++;
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

    return std::pair<std::vector<int>, int>(foundIndexVector, numberOfWords);
}

int main()
{
    std::pair<std::vector<int>, int> results = searchWord("radosny rabarbar rabat rower rabarbar", "rabarbar");
    printf("Number of results: %d\n", results.second);
    std::for_each(results.first.begin(), results.first.end(), [](int i)
                  { printf("%d\n", i); });
    return 0;
}