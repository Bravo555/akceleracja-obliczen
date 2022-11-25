#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <string>
#include <iostream>
#include "file_utils.hpp"
#include "timer.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

// Helper function to print vector elements
template<typename T>
void printVector(const std::string& arrayName, const std::vector<T>& arrayData) {
    cout << arrayName << ":\t" << endl;
    for(int i = 0; i < arrayData.size(); ++i)
        cout << arrayData[i] << ' ';
    cout << endl;
}

string resultToStr(const std::vector<cl_uchar>& arrayData) {
    string resStr = "";
    string separator = ";";
    for (int i = 0; i < arrayData.size(); ++i) {
        if (arrayData[i] == 1) {
            resStr += std::to_string(i);
            resStr += separator;
        }
    }
    return resStr;
}

cl::Program createProgramFromFile(cl::Context& context, const string& fileName) {
    string kernelStr = readStrFromFile(fileName);
    cl::Program::Sources sources(1, std::make_pair(kernelStr.c_str(), kernelStr.length()));
    cl::Program program(context, sources);
    return program;
}

int main(int argc, char * argv[]) {
    string textFile = argv[1];
    string keywordFile = argv[2];
    string resultsFile = argv[3];
    Timer timer;

    cl::Program program;
    std::vector<cl::Device> devices;
    try {
        // Find the platform
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Create an OpenCL context
        cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
        cl::Context context(CL_DEVICE_TYPE_GPU, cps);

        // Detect OpenCL devices
        devices = context.getInfo<CL_CONTEXT_DEVICES>();

        // Create an OpenCL command queue
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        string text = readStrFromFile(textFile);
        string keyword = readStrFromFile(keywordFile);

        std::vector<cl_uchar> indices(text.size());

        // Create OpenCL memory buffers
        cl::Buffer bufText(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * (int)text.size(), (char*)text.data());
        cl::Buffer bufKeyword(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * (int)keyword.size(), (char*)keyword.data());
        cl::Buffer bufIndices(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar) * (int)indices.size(), indices.data());

        // Load CL file, build CL program object, create CL kernel object
        // Modified to use a version copied by Cmake into build directory
        program = createProgramFromFile(context, "../string_search_kernel.cl");
        program.build(devices);
        cl::Kernel kernel = cl::Kernel(program, "string_search");

        // Set the arguments that will be used for kernel execution
        kernel.setArg(0, bufText);
        kernel.setArg(1, bufKeyword);
        kernel.setArg(2, (unsigned int)keyword.size());
        kernel.setArg(3, bufIndices);
        
        timer.start();
        // Print the input text
        // printVector("input", std::vector<char>(text.begin(), text.end()));

        // Enqueue the kernel to the queue
        // with appropriate global and local work sizes
        queue.enqueueNDRangeKernel(kernel, cl::NDRange(0), cl::NDRange(text.size()));

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
        
        string resultsStr = resultToStr(results);
        writeResultToFile(elapsedUs, resultsStr, resultsFile);
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
    catch(string msg)
    {
        cerr << "Exception caught in main(): " << msg << endl;
    }
}
