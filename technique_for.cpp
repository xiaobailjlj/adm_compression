#include "global.h"
#include <fstream>
#include <iostream>
#include <vector>

void compress_for(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
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

    // take samples and find the vague midian
    int sampleSize = 20;
    int count = 0;
    std::vector<int64_t> sample;

    while (inputFile >> value) {
        std::cout << "value:" << value << "\n";
        if (sample.size() < sampleSize) {
            sample.push_back(value);
        } else {
            int randIndex = std::rand() % (++count);
            if (randIndex < sampleSize) {
                sample[randIndex] = value;
            }
        }
        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
    }
    std::sort(sample.begin(), sample.end());
    int n = sample.size();
    int64_t midianValue = (n % 2 == 1) ? sample[n / 2] : floor((sample[n / 2 - 1] + sample[n / 2]) / 2.0);
    int64_t meanValue = floor((minValue + maxValue)/2);
    
    // set and write base value, min or mean or midian(hard to find without sorting, determined by sampling)
    int64_t baseValue = midianValue;
    int64_t minSubBase = minValue - baseValue;
    int64_t maxSubBase = maxValue - baseValue;

    outputFile.write(reinterpret_cast<char*>(&baseValue), sizeof(baseValue));


    std::cout << "minValue:" << minValue << "\n";
    std::cout << "maxValue:" << maxValue << "\n";
    std::cout << "meanValue:" << meanValue << "\n";
    std::cout << "midianValue:" << midianValue << "\n";
    std::cout << "baseValue:" << baseValue << "\n";
    std::cout << "minSubBase:" << minSubBase << "\n";
    std::cout << "maxSubBase:" << maxSubBase << "\n";


    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    // determain and mark actual dataType
    std::string dataType;
    int8_t  dataTypeInt = 0;
    if (minSubBase >= std::numeric_limits<int8_t>::min() && maxSubBase <= std::numeric_limits<int8_t>::max()) {
        dataType = "int8";
        dataTypeInt = 1;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    } else if (minSubBase >= std::numeric_limits<int16_t>::min() && maxSubBase <= std::numeric_limits<int16_t>::max()) {
        dataType = "int16";
        dataTypeInt = 2;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    } else if (minSubBase >= std::numeric_limits<int32_t>::min() && maxSubBase <= std::numeric_limits<int32_t>::max()) {
        dataType = "int32";
        dataTypeInt = 3;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    } else if (minSubBase >= std::numeric_limits<int64_t>::min() && maxSubBase <= std::numeric_limits<int64_t>::max()) {
        dataType = "int64";
        dataTypeInt = 4;
        outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));
    }
    std::cout << "dataType:" << dataType << "\n";
    std::cout << "dataTypeInt: " << static_cast<int>(dataTypeInt) << "\n"; 


    if (dataType == "int8") {
        // origin value might be big, use int64_t
        int64_t valueOut;
        while (inputFile >> valueOut) {
            std::cout << "ori:" << valueOut << "\n";
            int8_t sub =  valueOut - baseValue;
            std::cout << "sub:" << sub << "\n";
            outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
        }
    } else if (dataType == "int16") {
        int64_t valueOut;
        while (inputFile >> valueOut) {
            std::cout << "ori:" << valueOut << "\n";
            int16_t sub =  valueOut - baseValue;
            std::cout << "sub:" << sub << "\n";
            outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
        }
    } else if (dataType == "int32") {
        int64_t valueOut;
        while (inputFile >> valueOut) {
            std::cout << "ori:" << valueOut << "\n";
            int32_t sub =  valueOut - baseValue;
            std::cout << "sub:" << sub << "\n";
            outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
        }
    } else if (dataType == "int64") {
        int64_t valueOut;
        while (inputFile >> valueOut) {
            std::cout << "ori:" << valueOut << "\n";
            int64_t sub =  valueOut - baseValue;
            std::cout << "sub:" << sub << "\n";
            outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}

void decompress_for(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    // read base value
    int64_t baseValue;
    inputFile.read(reinterpret_cast<char*>(&baseValue), sizeof(baseValue));

    // read data type
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
            std::cout << "value:" << static_cast<int>(value) << "\n";
            // can't tell int size after adding base value, set to int64
            int64_t valueAddBase = value + baseValue;
            std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
            outputFile << static_cast<int>(valueAddBase) << "\n";
        }
    } else if (dataType == "int16") {
        int16_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            int64_t valueAddBase = value + baseValue;
            std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
            outputFile << static_cast<int>(valueAddBase) << "\n";
        }
    } else if (dataType == "int32") {
        int32_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            int64_t valueAddBase = value + baseValue;
            std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
            outputFile << static_cast<int>(valueAddBase) << "\n";
        }
    } else if (dataType == "int64") {
        int64_t value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            int64_t valueAddBase = value + baseValue;
            std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
            outputFile << static_cast<int>(valueAddBase) << "\n";
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}