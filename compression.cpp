#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include "global.h"

// make
// run compress: ./compression_program en bin int64 ./data/l_discount-int64.csv ./output/l_discount-int64.bin
// run decompress: ./compression_program de bin int64 ./output/l_discount-int64.bin ./output/l_discount-int64.csv 

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cerr << "Usage:\n"
                  << "./compression_program en|de bin|rle|dic|for|dif int8|int16|int32|int64|string input_file_path output_file_path\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string technique = argv[2];
    std::string dataType = argv[3];
    std::string inputFilePath = argv[4];
    std::string outputFilePath = argv[5];
    

    // param check
    if (mode != "en" && mode != "de")
    {
        std::cerr << "Invalid mode.\n";
        return 1;
    }

    if (technique != "bin" && technique != "rle" && technique != "dic" && technique != "for" && technique != "dif")
    {
        std::cerr << "Invalid compression technique.\n";
        return 1;
    }

    if (dataType != "int8" && dataType != "int16" && dataType != "int32" && dataType != "int64" && dataType != "string")
    {
        std::cerr << "Invalid data type.\n";
        return 1;
    }

    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    // logic
    if (mode == "en")
    {
        if(technique == "bin")
        {
            compress_bin(dataType, inputFilePath, outputFilePath);
        } 
        else if(technique == "rle")
        {
            compress_rle(dataType, inputFilePath, outputFilePath);
        } 
        else if(technique == "dic")
        {
            compress_dic(dataType, inputFilePath, outputFilePath);
        } 
        else if(technique == "for")
        {
            compress_for(dataType, inputFilePath, outputFilePath);
        } 
        else if(technique == "dif")
        {
            compress_dif(dataType, inputFilePath, outputFilePath);
        }
    }
    else if (mode == "de")
    {
        if(technique == "bin")
        {
            decompress_bin(dataType, inputFilePath, outputFilePath);
        } 
         else if(technique == "rle")        
        {
             decompress_rle(dataType, inputFilePath, outputFilePath);
        } 
         else if(technique == "dic")
         {
             decompress_dic(dataType, inputFilePath, outputFilePath);
        } 
        else if(technique == "for")
        {
            decompress_for(dataType, inputFilePath, outputFilePath);
        } 
        else if(technique == "dif")
        {
            decompress_dif(dataType, inputFilePath, outputFilePath);
        }
    }
    else
    {
        std::cerr << "Invalid mode.\n";
        return 1;
    }

    // Record end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate duration
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time for: " << mode << ", " << technique << ", "<< dataType << ", "<< inputFilePath << ", "<< outputFilePath  << " is: " << duration.count() << " seconds\n";
    
    return 0;
}
