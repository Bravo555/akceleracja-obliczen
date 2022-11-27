#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "kmp.cpp"

#include "../common/file_utils.hpp"
#include "../common/timer.hpp"

int main(int argc, char * argv[]) {
    if (argc != 5) {
        std::cout << "ERROR! Wrong number of arguments!" << std::endl;
        std::cout << "Usage: " << argv[0] << " TEXT PATTERN RESULT" << std::endl;
        return -1;
    }
    std::string textFile = argv[1];
    std::string keywordFile = argv[2];
    std::string resultsFile = argv[3];
    std::string isMultiThreaded = "";
    if (argc > 3)
    {
        isMultiThreaded = argv[4];
    }
    Timer timer;

    std::string text = readStrFromFile(textFile);
    std::string keyword = readStrFromFile(keywordFile);
    std::vector<int> results;

    timer.start();
    if (isMultiThreaded == "multithread")
    {
        results = multiThreadKMP(text, keyword);
    }
    else
    {
        results = singleThreadKMP(text, keyword);
    }
    long long elapsedUs = timer.getElapsedMicroseconds();
    writeResultToFile(elapsedUs, results, resultsFile);

    return 0;
}
