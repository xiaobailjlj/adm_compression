#include "global.h"
#include <fstream>
#include <iostream>

void compress_for(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    if (dataType == "int8") {
        int8_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else if (dataType == "int16") {
        int16_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else if (dataType == "int32") {
        int32_t value;
        while (inputFile >> value) {
            outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    } else if (dataType == "int64") {
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

void decompress_for(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath) {
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