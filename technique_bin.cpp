#include "global.h"
#include <fstream>
#include <iostream>

// ./compression_program en bin int64 ./data/aa_try.csv ./output/bin/aa_try.bin
// ./compression_program de bin int8 ./output/bin/aa_try.bin ./output/bin/aa_try.csv


void compress_bin(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    // adapt dataType, automatically select a smaller integer type for compression
    int64_t value;
    int64_t minValue = std::numeric_limits<int8_t>::max();
    int64_t maxValue = std::numeric_limits<int8_t>::min();

    while (inputFile >> value) {
        // std::cout << "value:" << value << "\n";
        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
    }

    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    // determain and mark actual dataType
    std::string dataType;
    int8_t  dataTypeInt = 0;
    if (minValue >= std::numeric_limits<int8_t>::min() && maxValue <= std::numeric_limits<int8_t>::max()) {
        dataType = "int8";
        dataTypeInt = 1;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    } else if (minValue >= std::numeric_limits<int16_t>::min() && maxValue <= std::numeric_limits<int16_t>::max()) {
        dataType = "int16";
        dataTypeInt = 2;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    } else if (minValue >= std::numeric_limits<int32_t>::min() && maxValue <= std::numeric_limits<int32_t>::max()) {
        dataType = "int32";
        dataTypeInt = 3;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    } else if (minValue >= std::numeric_limits<int64_t>::min() && maxValue <= std::numeric_limits<int64_t>::max()) {
        dataType = "int64";
        dataTypeInt = 4;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    }
    std::cout << "dataType:" << dataType << "\n";
    std::cout << "dataTypeInt: " << static_cast<int>(dataTypeInt) << "\n"; 


    if (dataType == "int8") {
        // !!! To ensure correct reading for all integer types, use a larger integer type (like int16_t) for reading, and then cast it down to int8_t if necessary
        int temp;
        while (inputFile >> temp) {
            // std::cout << "valueOut temp:" << temp << "\n";
            int8_t valueOut = temp;
            // std::cout << "valueOut:" << static_cast<int>(valueOut) << "\n";
            outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
        }
    } else if (dataType == "int16") {
        int16_t valueOut;
        while (inputFile >> valueOut) {
            // std::cout << "valueOut:" << valueOut << "\n";
            outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
        }
    } else if (dataType == "int32") {
        int32_t valueOut;
        while (inputFile >> valueOut) {
            // std::cout << "valueOut:" << valueOut << "\n";
            outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
        }
    } else if (dataType == "int64") {
        int64_t valueOut;
        while (inputFile >> valueOut) {
            // std::cout << "valueOut:" << valueOut << "\n";
            outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}

void decompress_bin(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    int8_t  dataTypeInt = 0;
    std::string dataType;
    inputFile.read(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    if (dataTypeInt==1) {
        dataType = "int8";
    } else if (dataTypeInt==2) {
        dataType = "int16";
    } else if (dataTypeInt==3) {
        dataType = "int32";
    } else if (dataTypeInt==4) {
        dataType = "int64";
    } else {
        throw std::invalid_argument("No DataType");
    }
    std::cout << "dataType:" << dataType << "\n";

    if (dataType == "int8") {
        int8_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            outputFile << static_cast<int>(value) << "\n";
        }
    } else if (dataType == "int16") {
        int16_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << static_cast<int>(value) << "\n";
        }
    } else if (dataType == "int32") {
        int32_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << static_cast<int>(value) << "\n";
        }
    } else if (dataType == "int64") {
        int64_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << static_cast<int>(value) << "\n";
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}