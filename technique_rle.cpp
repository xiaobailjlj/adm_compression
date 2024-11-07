#include "global.h"
#include <fstream>
#include <iostream>
#include <vector>

// RLE compression and decompression for int8, int16, int32, int64, and string types

void compress_rle(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Start RLE compression.\n";
    std::ifstream inputFile(inputFilePath, std::ios::binary);  // Open input file in binary mode
    std::ofstream outputFile(outputFilePath, std::ios::binary); // Open output file in binary mode

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    // Determine data type
    bool isIntegerType = true;
    std::string dataType = dataTypeOri;
    if (dataType == "string") {
        isIntegerType = false;
    }

    // Vector to store run-length encoding data
    std::vector<std::pair<int64_t, uint64_t>> runs_int;
    std::vector<std::pair<std::string, uint64_t>> runs_str;
    int64_t min_value = INT64_MAX;
    int64_t max_value = INT64_MIN;
    uint64_t max_count = 0;

    // Read data and collect run lengths
    if (isIntegerType) {
        int64_t prevValue = 0, currentValue;
        uint64_t count = 0;
        bool firstValue = true;
        std::string line;
        while (std::getline(inputFile, line, '\n')) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) continue; 
            currentValue = std::stoll(line);
            if (firstValue) {
                prevValue = currentValue;
                count = 1;
                firstValue = false;
            } else {
                if (currentValue == prevValue) {
                    count++;
                } else {
                    runs_int.push_back({prevValue, count});
                    max_count = std::max(max_count, count);
                    min_value = std::min(min_value, prevValue);
                    max_value = std::max(max_value, prevValue);
                    prevValue = currentValue;
                    count = 1;
                }
            }
        }
        if (!firstValue) {
            runs_int.push_back({prevValue, count});
            max_count = std::max(max_count, count);
            min_value = std::min(min_value, prevValue);
            max_value = std::max(max_value, prevValue);
        }
    } else {
        std::string prevValue, currentValue;
        uint64_t count = 0;
        bool firstValue = true;
        while (std::getline(inputFile, currentValue, '\n')) {
            if (!currentValue.empty() && currentValue.back() == '\r') {
                currentValue.pop_back();
            }
            if (firstValue) {
                prevValue = currentValue;
                count = 1;
                firstValue = false;
            } else {
                if (currentValue == prevValue) {
                    count++;
                } else {
                    runs_str.push_back({prevValue, count});
                    max_count = std::max(max_count, count);
                    prevValue = currentValue;
                    count = 1;
                }
            }
        }
        if (!firstValue) {
            runs_str.push_back({prevValue, count});
            max_count = std::max(max_count, count);
        }
    }
    inputFile.close();

    // Determine the smallest data type
    std::string valueDataType, countDataType;
    if (isIntegerType) {
        if (min_value >= INT8_MIN && max_value <= INT8_MAX) {
            valueDataType = "int8";
        } else if (min_value >= INT16_MIN && max_value <= INT16_MAX) {
            valueDataType = "int16";
        } else if (min_value >= INT32_MIN && max_value <= INT32_MAX) {
            valueDataType = "int32";
        } else {
            valueDataType = "int64";
        }
    } else {
        valueDataType = "string";
    }
    if (max_count <= UINT8_MAX) {
        countDataType = "uint8";
    } else if (max_count <= UINT16_MAX) {
        countDataType = "uint16";
    } else if (max_count <= UINT32_MAX) {
        countDataType = "uint32";
    } else {
        countDataType = "uint64";
    }

    // Write header information
    int8_t valueDataTypeInt;
    if (valueDataType == "int8") valueDataTypeInt = 1;
    else if (valueDataType == "int16") valueDataTypeInt = 2;
    else if (valueDataType == "int32") valueDataTypeInt = 3;
    else if (valueDataType == "int64") valueDataTypeInt = 4;
    else valueDataTypeInt = 5;
    int8_t countDataTypeInt;
    if (countDataType == "uint8") countDataTypeInt = 1;
    else if (countDataType == "uint16") countDataTypeInt = 2;
    else if (countDataType == "uint32") countDataTypeInt = 3;
    else countDataTypeInt = 4;
    outputFile.write(reinterpret_cast<char*>(&valueDataTypeInt), sizeof(valueDataTypeInt));
    outputFile.write(reinterpret_cast<char*>(&countDataTypeInt), sizeof(countDataTypeInt));

    // Write run-length encoded data
    if (isIntegerType) {
        for (const auto& run : runs_int) {
            int64_t value = run.first;
            uint64_t count = run.second;
            if (valueDataType == "int8") {
                int8_t v = static_cast<int8_t>(value);
                outputFile.write(reinterpret_cast<char*>(&v), sizeof(v));
            } else if (valueDataType == "int16") {
                int16_t v = static_cast<int16_t>(value);
                outputFile.write(reinterpret_cast<char*>(&v), sizeof(v));
            } else if (valueDataType == "int32") {
                int32_t v = static_cast<int32_t>(value);
                outputFile.write(reinterpret_cast<char*>(&v), sizeof(v));
            } else {
                outputFile.write(reinterpret_cast<char*>(&value), sizeof(value));
            }
            if (countDataType == "uint8") {
                uint8_t c = static_cast<uint8_t>(count);
                outputFile.write(reinterpret_cast<char*>(&c), sizeof(c));
            } else if (countDataType == "uint16") {
                uint16_t c = static_cast<uint16_t>(count);
                outputFile.write(reinterpret_cast<char*>(&c), sizeof(c));
            } else if (countDataType == "uint32") {
                uint32_t c = static_cast<uint32_t>(count);
                outputFile.write(reinterpret_cast<char*>(&c), sizeof(c));
            } else {
                outputFile.write(reinterpret_cast<char*>(&count), sizeof(count));
            }
        }
    } else {
        for (const auto& run : runs_str) {
            std::string value = run.first;
            uint64_t count = run.second;
            uint16_t strLength = static_cast<uint16_t>(value.length());
            outputFile.write(reinterpret_cast<char*>(&strLength), sizeof(strLength));
            outputFile.write(value.c_str(), strLength);
            if (countDataType == "uint8") {
                uint8_t c = static_cast<uint8_t>(count);
                outputFile.write(reinterpret_cast<char*>(&c), sizeof(c));
            } else if (countDataType == "uint16") {
                uint16_t c = static_cast<uint16_t>(count);
                outputFile.write(reinterpret_cast<char*>(&c), sizeof(c));
            } else if (countDataType == "uint32") {
                uint32_t c = static_cast<uint32_t>(count);
                outputFile.write(reinterpret_cast<char*>(&c), sizeof(c));
            } else {
                outputFile.write(reinterpret_cast<char*>(&count), sizeof(count));
            }
        }
    }
    outputFile.close();
    std::cout << "RLE compression completed.\n";
}

void decompress_rle(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Start RLE decompression.\n";
    std::ifstream inputFile(inputFilePath, std::ios::binary); 
    std::ofstream outputFile(outputFilePath, std::ios::binary); 

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    // Read header information
    int8_t valueDataTypeInt, countDataTypeInt;
    inputFile.read(reinterpret_cast<char*>(&valueDataTypeInt), sizeof(valueDataTypeInt));
    inputFile.read(reinterpret_cast<char*>(&countDataTypeInt), sizeof(countDataTypeInt));
    std::string valueDataType, countDataType;
    if (valueDataTypeInt == 1) valueDataType = "int8";
    else if (valueDataTypeInt == 2) valueDataType = "int16";
    else if (valueDataTypeInt == 3) valueDataType = "int32";
    else if (valueDataTypeInt == 4) valueDataType = "int64";
    else valueDataType = "string";
    if (countDataTypeInt == 1) countDataType = "uint8";
    else if (countDataTypeInt == 2) countDataType = "uint16";
    else if (countDataTypeInt == 3) countDataType = "uint32";
    else countDataType = "uint64";

    // Read and reconstruct data
    bool isIntegerType = (valueDataType != "string");
    if (isIntegerType) {
        while (inputFile.peek() != EOF) {
            int64_t value;
            uint64_t count;
            if (valueDataType == "int8") {
                int8_t v;
                inputFile.read(reinterpret_cast<char*>(&v), sizeof(v));
                value = v;
            } else if (valueDataType == "int16") {
                int16_t v;
                inputFile.read(reinterpret_cast<char*>(&v), sizeof(v));
                value = v;
            } else if (valueDataType == "int32") {
                int32_t v;
                inputFile.read(reinterpret_cast<char*>(&v), sizeof(v));
                value = v;
            } else {
                inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
            }
            if (countDataType == "uint8") {
                uint8_t c;
                inputFile.read(reinterpret_cast<char*>(&c), sizeof(c));
                count = c;
            } else if (countDataType == "uint16") {
                uint16_t c;
                inputFile.read(reinterpret_cast<char*>(&c), sizeof(c));
                count = c;
            } else if (countDataType == "uint32") {
                uint32_t c;
                inputFile.read(reinterpret_cast<char*>(&c), sizeof(c));
                count = c;
            } else {
                inputFile.read(reinterpret_cast<char*>(&count), sizeof(count));
            }
            for (uint64_t i = 0; i < count; ++i) {
                outputFile << value << '\n'; 
            }
        }
    } else {
        while (inputFile.peek() != EOF) {
            uint16_t strLength;
            inputFile.read(reinterpret_cast<char*>(&strLength), sizeof(strLength));
            char* buffer = new char[strLength + 1];
            inputFile.read(buffer, strLength);
            buffer[strLength] = '\0';
            std::string value(buffer);
            delete[] buffer;
            uint64_t count;
            if (countDataType == "uint8") {
                uint8_t c;
                inputFile.read(reinterpret_cast<char*>(&c), sizeof(c));
                count = c;
            } else if (countDataType == "uint16") {
                uint16_t c;
                inputFile.read(reinterpret_cast<char*>(&c), sizeof(c));
                count = c;
            } else if (countDataType == "uint32") {
                uint32_t c;
                inputFile.read(reinterpret_cast<char*>(&c), sizeof(c));
                count = c;
            } else {
                inputFile.read(reinterpret_cast<char*>(&count), sizeof(count));
            }
            for (uint64_t i = 0; i < count; ++i) {
                outputFile << value << '\n'; 
            }
        }
    }
    inputFile.close();
    outputFile.close();
    std::cout << "RLE decompression completed.\n";
}
