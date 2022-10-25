#include <iostream>
#include <string.h>
#include <vector>

std::pair<std::vector<int>, int> searchWord(char text[], char soughtWord[])
{
    std::vector<int> foundIndexVector = {};
    int numberOfWords = 0;
    int currPosInText = 0;
    int currPosInWord = 0;
    std::vector<int> partialMatchTable[] = {};

    int soughtWordLen = strlen(soughtWord);

    while (currPosInText < soughtWordLen)
    {
        if (soughtWord[currPosInWord] == text[currPosInText])
        {
            currPosInText++;
            currPosInWord++;
            if (currPosInWord == soughtWordLen)
            {
                foundIndexVector.push_back(currPosInText - currPosInWord);
                numberOfWords++;
                currPosInWord = partialMatchTable->at(currPosInWord);
            }
            else
            {
                currPosInWord = partialMatchTable->at(currPosInWord);
                if (currPosInWord < 0)
                {
                    currPosInText++;
                    currPosInWord++;
                }
            }
        }
    }

    return std::pair<std::vector<int>, int>(foundIndexVector, numberOfWords);
}

int main()
{
    searchWord("AAAAAA", "BB");
    return 0;
}