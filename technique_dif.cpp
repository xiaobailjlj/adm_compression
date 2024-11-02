#include "global.h"
#include <fstream>
#include <iostream>
#include <vector>

// ./compression_program en for int8 ./data/l_discount-int8.csv ./output/dif/l_discount-int8d.dif
// ./compression_program en for int16 ./data/l_discount-int16.csv ./output/dif/l_discount-int16d.dif
// ./compression_program en for int32 ./data/l_discount-int32.csv ./output/dif/l_discount-int32d.dif
// ./compression_program en for int64 ./data/l_discount-int32.csv ./output/dif/l_discount-int64d.dif



// ./compression_program de for int8 ./output/dif/l_discount-int8d.dif ./output/dif/l_discount-int8.csv
// ./compression_program de for int16 ./output/dif/l_discount-int16d.dif ./output/dif/l_discount-int16.csv
// ./compression_program de for int32 ./output/dif/l_discount-int32d.dif ./output/dif/l_discount-int32.csv
// ./compression_program de for int64 ./output/dif/l_discount-int32d.dif ./output/dif/l_discount-int32.csv



// ./compression_program en for int64 ./data/aa_try.csv ./output/dif/aa_tryd.dif
// ./compression_program de for int8 ./output/dif/aa_tryd.dif ./output/dif/aa_try.csv


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
    std::string dataType = "int8";
    int8_t  dataTypeInt = 1;
    
    int64_t max_positive = (1 << (target_bits - 1)) - 1;  // For 4 bits: 7
    int64_t min_negative = -(1 << (target_bits - 1));     // For 4 bits: -8
    
    // reserve highest positive and lowest negative as escape codes
    int64_t positive_escape = max_positive;
    // int64_t negative_escape = min_negative;
    
    std::cout << "max_positive:" << max_positive << "\n";
    std::cout << "min_negative:" << min_negative << "\n";
    std::cout << "dataType:" << dataType << "\n";
    std::cout << "dataTypeInt: " << static_cast<int>(dataTypeInt) << "\n"; 

    outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));

    // set the first base value
    int64_t currentBaseValue;
    inputFile >> currentBaseValue;
    outputFile.write(reinterpret_cast<char*>(&currentBaseValue), sizeof(currentBaseValue));

    // origin value might be big, use int64_t
    int64_t valueOut;
    int64_t valueDiff;

    if (dataType == "int8") {
        int8_t sub;
        while (inputFile >> valueOut) {
            valueDiff = valueOut - currentBaseValue;
            currentBaseValue = valueOut;
            std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                sub = positive_escape;
                std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else if (dataType == "int16") {
        int16_t sub;
        while (inputFile >> valueOut) {
            valueDiff = valueOut - currentBaseValue;
            currentBaseValue = valueOut;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                sub = positive_escape;
                // std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else if (dataType == "int32") {
        int32_t sub;
        while (inputFile >> valueOut) {
            valueDiff = valueOut - currentBaseValue;
            currentBaseValue = valueOut;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
                sub = positive_escape;
                // std::cout << "excape \n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
                outputFile.write(reinterpret_cast<char*>(&valueOut), sizeof(valueOut));
            }
        }
    } else if (dataType == "int64") {
        int64_t sub;
        while (inputFile >> valueOut) {
            valueDiff = valueOut - currentBaseValue;
            currentBaseValue = valueOut;
            // std::cout << "ori:" << valueOut << "\n";
            if ((min_negative <= valueDiff) && (valueDiff <= max_positive) ){
                sub =  valueDiff;
                // std::cout << "sub:" << static_cast<int>(sub) << "\n";
                outputFile.write(reinterpret_cast<char*>(&sub), sizeof(sub));
            }
            else {
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

    if (dataType == "int8") {
        int8_t value;
        int64_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            uint8_t target_bits = 8;
            int8_t max_positive = (1 << (target_bits - 1)) - 1;
            if (value!=max_positive){
                // can't determine int size after adding base value, set to int64
                int64_t valueAddBase = value + baseValue;
                baseValue = valueAddBase;
                std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
                baseValue = value_escape;
                std::cout << "escape value:" << static_cast<int>(value_escape) << "\n";
                outputFile << static_cast<int>(value_escape) << "\n";
            }
        }
    } else if (dataType == "int16") {
        int16_t value;
        int64_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            uint8_t target_bits = 16;
            int8_t max_positive = (1 << (target_bits - 1)) - 1;
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
        int32_t value;
        int64_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            uint8_t target_bits = 32;
            int8_t max_positive = (1 << (target_bits - 1)) - 1;
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
        int64_t value;
        int64_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            uint8_t target_bits = 64;
            int8_t max_positive = (1 << (target_bits - 1)) - 1;
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