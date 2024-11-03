#include "global.h"
#include <fstream>
#include <iostream>
#include <vector>

// ./compression_program en for int64 ./data/aa_try.csv ./output/dif/aa_try.dif
// ./compression_program de for int8 ./output/dif/aa_try.dif ./output/dif/aa_try.csv


void compress_dif(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Start compression. \n";
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    // determine target bit width(dataType), and calculate the maxmum and minimum sub value, subs exceed this range are exceptions
    uint8_t target_bits = 8;
    
    // reserve highest positive and lowest negative as escape codes
    int64_t max_positive = (1 << (target_bits - 1)) - 1 - 1;  // For 4 bits: 7
    int64_t min_negative = -(1 << (target_bits - 1)) + 1;     // For 4 bits: -8
    
    // escape codes
    int64_t positive_escape =  (1 << (target_bits - 1)) - 1;
    // int64_t negative_escape = -(1 << (target_bits - 1));
    
    std::cout << "max_positive:" << max_positive << "\n";
    std::cout << "min_negative:" << min_negative << "\n";


    // determain and mark actual dataType(for excape values)
    int64_t value;
    int64_t minValue = std::numeric_limits<int8_t>::max() - 1;
    int64_t maxValue = std::numeric_limits<int8_t>::min() + 1;

    while (inputFile >> value) {
        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
    }
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

    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);


    // set the first base value
    int64_t currentBaseValue;
    inputFile >> currentBaseValue;
    outputFile.write(reinterpret_cast<char*>(&currentBaseValue), sizeof(currentBaseValue));

    // origin value might be big, use int64_t
    int64_t valueOut_ori;
    int64_t valueDiff;

    int8_t sub;
    if (dataType == "int8") {
        while (inputFile >> valueOut_ori) {
            valueDiff = valueOut_ori - currentBaseValue;
            currentBaseValue = valueOut_ori;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                int8_t valueOut = valueOut_ori;
                sub = positive_escape;
                // std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else if (dataType == "int16") {
        while (inputFile >> valueOut_ori) {
            valueDiff = valueOut_ori - currentBaseValue;
            currentBaseValue = valueOut_ori;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                int16_t valueOut = valueOut_ori;
                sub = positive_escape;
                // std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else if (dataType == "int32") {
        while (inputFile >> valueOut_ori) {
            // if ((currentBaseValue == 7914)&&(valueOut_ori=8041)){
            //     bool flag = true;
            // };
            valueDiff = valueOut_ori - currentBaseValue;
            currentBaseValue = valueOut_ori;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                int32_t valueOut = valueOut_ori;
                sub = positive_escape;
                // std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else if (dataType == "int64") {
        while (inputFile >> valueOut_ori) {
            valueDiff = valueOut_ori - currentBaseValue;
            currentBaseValue = valueOut_ori;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                int64_t valueOut = valueOut_ori;
                sub = positive_escape;
                // std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}

void decompress_dif(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Start decompression. \n";
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    // read data type
    int8_t  dataTypeInt = 0;
    std::string dataType;
    inputFile.read(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));

    // read base value
    int64_t baseValue;
    inputFile.read(reinterpret_cast<char*>(&baseValue), sizeof(baseValue));
    outputFile << static_cast<int>(baseValue) << "\n";


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


    int8_t value;
    uint8_t target_bits = 8;
    int8_t max_positive = (1 << (target_bits - 1)) - 1;
    if (dataType == "int8") {
        int8_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            if (value!=max_positive){
                // can't determine int size after adding base value, set to int64
                int64_t valueAddBase = value + baseValue;
                baseValue = valueAddBase;
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
                baseValue = value_escape;
                // std::cout << "escape value:" << static_cast<int>(value_escape) << "\n";
                outputFile << static_cast<int>(value_escape) << "\n";
            }
        }
    } else if (dataType == "int16") {
        int16_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            if (value!=max_positive){
                // can't determine int size after adding base value, set to int64
                int64_t valueAddBase = value + baseValue;
                baseValue = valueAddBase;
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
                baseValue = value_escape;
                // std::cout << "value:" << static_cast<int>(value_escape) << "\n";
                outputFile << static_cast<int>(value_escape) << "\n";
            }
        }
    } else if (dataType == "int32") {
        int32_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            if (value!=max_positive){
                // can't determine int size after adding base value, set to int64
                int64_t valueAddBase = value + baseValue;
                baseValue = valueAddBase;
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
                baseValue = value_escape;
                // std::cout << "value:" << static_cast<int>(value_escape) << "\n";
                outputFile << static_cast<int>(value_escape) << "\n";
            }
        }
    } else if (dataType == "int64") {
        int64_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            if (value!=max_positive){
                // can't determine int size after adding base value, set to int64
                int64_t valueAddBase = value + baseValue;
                baseValue = valueAddBase;
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
                baseValue = value_escape;
                // std::cout << "value:" << static_cast<int>(value_escape) << "\n";
                outputFile << static_cast<int>(value_escape) << "\n";
            }
        }
    } else {
        throw std::invalid_argument("Unsupported DataType");
    }

    inputFile.close();
    outputFile.close();
}