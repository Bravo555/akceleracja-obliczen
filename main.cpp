#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>

std::pair<std::vector<int>, int> searchWord(char text[], char soughtWord[])
{
    int soughtWordLen = (int)strlen(soughtWord);
    std::vector<int> foundIndexVector = {};
    int numberOfWords = 0;
    int currPosInText = 0;
    int currPosInWord = 0;
    std::vector<int> partialMatchTable(soughtWordLen);

    while (currPosInText < (int)strlen(text))
    {
        printf("TEXT: %d\nWORD: %d\n", currPosInText, currPosInWord);
        if (soughtWord[currPosInWord] == text[currPosInText])
        {
            printf("%", text[currPosInText]);
            currPosInText++;
            currPosInWord++;
            if (currPosInWord == soughtWordLen)
            {
                foundIndexVector.push_back(currPosInText - currPosInWord);
                numberOfWords++;
                printf("A\n");
                // currPosInWord = partialMatchTable.at(currPosInWord - 1);
                currPosInWord = 0;
                printf("B\n");
            }
        }
        else
        {
            // currPosInWord = partialMatchTable.at(currPosInWord);
            currPosInWord = 0;
            if (currPosInWord < 0)
            {
                currPosInWord++;
            }
            currPosInText++;
        }
    }

    return std::pair<std::vector<int>, int>(foundIndexVector, numberOfWords);
}

int main()
{
    std::pair<std::vector<int>, int> results = searchWord("DC ABC AB AC", "AB");
    printf("AFTER\n");
    printf("%d\n", results.second);
    std::for_each(results.first.begin(), results.first.end(), [](int i)
                  { printf("%d\n", i); });
    return 0;
}