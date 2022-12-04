#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <string>
#include <iostream>
#include "../common/file_utils.hpp"
#include "../common/timer.hpp"
#include <fstream>
#include <sstream>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

// Helper function to print vector elements
template <typename T>
void printVector(const std::string &arrayName, const std::vector<T> &arrayData)
{
    cout << arrayName << ":\t" << endl;
    for (int i = 0; i < arrayData.size(); ++i)
        cout << arrayData[i] << ' ';
    cout << endl;
}

// Convert result from bool vector to vector of indices of matches
std::vector<size_t> getVectorOfMatches(const std::vector<uint8_t> &arrayData)
{
    std::vector<size_t> matches;
    for (int i = 0; i < arrayData.size(); ++i)
    {
        if (arrayData[i] == 1)
        {
            matches.push_back(i);
        }
    }
    return matches;
}

cl::Program createProgramFromFile(cl::Context &context, const string &fileName)
{
    string kernelStr = readStrFromFile(fileName);
    cl::Program::Sources sources(1, std::make_pair(kernelStr.c_str(), kernelStr.length()));
    cl::Program program(context, sources);
    return program;
}

std::vector<int16_t> buildPartialMatchTable(const std::string &keyword)
{
    if (keyword.length() > INT16_MAX)
        return std::vector<int16_t>();

    std::vector<int16_t> partialMatchTable(keyword.length() + 1);
    partialMatchTable.at(0) = -1;
    int16_t currPos = 1;
    int16_t indexOfNextChar = 0;

    while (currPos < keyword.length())
    {
        if (keyword.at(currPos) == keyword.at(indexOfNextChar))
        {
            partialMatchTable.at(currPos) = partialMatchTable.at(indexOfNextChar);
        }
        else
        {
            partialMatchTable.at(currPos) = indexOfNextChar;
            while (indexOfNextChar >= 0 && keyword.at(currPos) != keyword.at(indexOfNextChar))
            {
                indexOfNextChar = partialMatchTable.at(indexOfNextChar);
            }
        }
        currPos++;
        indexOfNextChar++;
    }
    partialMatchTable.at(currPos) = indexOfNextChar;

    return partialMatchTable;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cout << "ERROR! Wrong number of arguments!" << std::endl;
        std::cout << "Usage: " << argv[0] << " KERNEL TEXT PATTERN RESULT" << std::endl;
        return -1;
    }

    string kernelFile = argv[1];
    string textFile = argv[2];
    string keywordFile = argv[3];
    string resultsFile = argv[4];
    Timer timer;

    const int iterNum = 20;

    cl::Program program;
    std::vector<cl::Device> devices;
    try
    {
        // Find the platform
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Create an OpenCL context
        cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, cps);

        // Detect OpenCL devices
        devices = context.getInfo<CL_CONTEXT_DEVICES>();

        // Create an OpenCL command queue
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        string text = readStrFromFile(textFile);
        string keyword = readStrFromFile(keywordFile);

        // If text length is not a multiple of 2 * keyword length, we pad the end with extra spaces so we can divide the
        // text into even blocks
        size_t blockSize = keyword.size() * 2;
        size_t remainder = text.size() % blockSize;

        for (size_t i = remainder; i < blockSize; ++i)
        {
            text += ' ';
        }

        std::vector<cl_uchar> indices(text.size());

        // Assume the keyword is shorter than 65 536 characters
        std::vector<int16_t> partialMatchTable = buildPartialMatchTable(keyword);
        if (partialMatchTable.size() == 0)
        {
            std::cout << "ERROR! Keyword too long" << std::endl;
            std::cout << "Usage: " << argv[0] << " KERNEL TEXT PATTERN RESULT" << std::endl;
            return -2;
        }

        // Create OpenCL memory buffers
        cl::Buffer bufText(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * (int)text.size(), (char *)text.data());
        cl::Buffer bufKeyword(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * (int)keyword.size(), (char *)keyword.data());
        cl::Buffer bufPartialMatchTable(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uint16_t) * partialMatchTable.size(), partialMatchTable.data());
        cl::Buffer bufIndices(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar) * (int)indices.size(), indices.data());

        // Load CL file, build CL program object, create CL kernel object
        // Modified to use a version copied by Cmake into build directory
        program = createProgramFromFile(context, kernelFile);
        program.build(devices);
        cl::Kernel kernel = cl::Kernel(program, "string_search");

        // Set the arguments that will be used for kernel execution
        kernel.setArg(0, bufText);
        kernel.setArg(1, bufKeyword);
        kernel.setArg(2, (unsigned int)keyword.size());
        kernel.setArg(3, bufPartialMatchTable);
        kernel.setArg(4, bufIndices);

        writeCsvHeader(resultsFile);

        for (int i = 0; i <= iterNum; ++i)
        {

            timer.start();
            // Print the input text
            // printVector("input", std::vector<char>(text.begin(), text.end()));

            // Enqueue the kernel to the queue with appropriate global and local work sizes
            //
            // String was padded, so blocks guaranteed to be the same size
            size_t blocks = text.size() / keyword.size() - 1;
            queue.enqueueNDRangeKernel(kernel, cl::NDRange(0), cl::NDRange(blocks));

            // Enqueue blocking call to read back indices buffer
            //
            // The output buffer is the same size as the input buffer and contains `1`s at indices where the input text
            // matches the keyword. It's specifically not a vector of bools, because C++ tries to be smart and optimize
            // the storage of the vector, basically making it so every bool takes up a bit instead of a byte, but we
            // explicitly need each element to take up a byte, so that's why we use `uint8_t` here.
            //
            // We could potentially make it so that for every byte in the input there is only 1 bit in the output, saving
            // some space that way, but we'd have to handle that in the kernel itself. Additionally, there can be no more
            // matches than `len(input) / len(keyword)` (assuming that our matches are guaranteed not to be aliased)
            // so we can also definitely improve our upper bound here, but I'd like to have some actual measurements before
            // engaging in potentially premature optimisation.
            std::vector<uint8_t> results(text.size(), 127);
            queue.enqueueReadBuffer(bufIndices, CL_TRUE, 0, sizeof(uint8_t) * results.size(), results.data());

            long long elapsedUs = timer.getElapsedMicroseconds();

            // Another hack to convert `uint8_t` to `uint16_t` because to C++ `uint8_t` is internally char therefore
            // character
            // printVector("indices", std::vector<uint16_t>(results.begin(), results.end()));

            std::vector<size_t> matches = getVectorOfMatches(results);
            appendResultToFile(elapsedUs, matches, resultsFile);
        }
    }
    catch (cl::Error err)
    {
        // Catch OpenCL errors and print log if it is a build error
        cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << endl;
        if (err.err() == CL_BUILD_PROGRAM_FAILURE)
        {
            string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
            cout << "Program Info: " << str << endl;
        }
    }
    catch (string msg)
    {
        cerr << "Exception caught in main(): " << msg << endl;
    }
}
