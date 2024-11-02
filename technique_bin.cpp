#include "global.h"
#include <fstream>
#include <iostream>

void compress_bin(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath) {
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
        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
    }

    
    std::string dataType_adapted;
    if (minValue >= std::numeric_limits<int8_t>::min() && maxValue <= std::numeric_limits<int8_t>::max()) {
        dataType_adapted = "int8";
    } else if (minValue >= std::numeric_limits<int16_t>::min() && maxValue <= std::numeric_limits<int16_t>::max()) {
        dataType_adapted = "int16_t";
    } else if (minValue >= std::numeric_limits<int32_t>::min() && maxValue <= std::numeric_limits<int32_t>::max()) {
        dataType_adapted = "int32_t";
    } else if (minValue >= std::numeric_limits<int64_t>::min() && maxValue <= std::numeric_limits<int64_t>::max()) {
        dataType_adapted = "int64_t";
    }

    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);


    if (dataType_adapted == "int8") {
        int8_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else if (dataType_adapted == "int16") {
        int16_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else if (dataType_adapted == "int32") {
        int32_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else if (dataType_adapted == "int64") {
        int64_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}

void decompress_bin(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    if (dataType == "int8") {
        int8_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << value << "\n";
        }
    } else if (dataType == "int16") {
        int16_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << value << "\n";
        }
    } else if (dataType == "int32") {
        int32_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << value << "\n";
        }
    } else if (dataType == "int64") {
        int64_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            outputFile << value << "\n";
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}