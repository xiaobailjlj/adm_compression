#include "global.h"
#include <fstream>
#include <iostream>
#include <vector>

// ./compression_program en for int8 ./data/l_discount-int8.csv ./output/for/l_discount-int8.for
// ./compression_program en for int16 ./data/l_discount-int16.csv ./output/for/l_discount-int16.for
// ./compression_program en for int32 ./data/l_discount-int32.csv ./output/for/l_discount-int32.for
// ./compression_program en for int64 ./data/l_discount-int32.csv ./output/for/l_discount-int64.for



// ./compression_program de for int8 ./output/for/l_discount-int8.for ./output/for/l_discount-int8.csv
// ./compression_program de for int16 ./output/for/l_discount-int16.for ./output/for/l_discount-int16.csv
// ./compression_program de for int32 ./output/for/l_discount-int32.for ./output/for/l_discount-int32.csv
// ./compression_program de for int64 ./output/for/l_discount-int32.for ./output/for/l_discount-int32.csv



// ./compression_program en for int64 ./data/aa_try.csv ./output/for/aa_try.for
// ./compression_program de for int8 ./output/for/aa_try.for ./output/for/aa_try.csv


void compress_for(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Start compression. \n";
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
        // std::cout << "value:" << value << "\n";
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

    // determine target bit width(dataType), and calculate the maxmum and minimum sub value, subs exceed this range are exceptions
    uint8_t target_bits = 8;
    std::string dataType = "int8";
    int8_t  dataTypeInt = 1;
    
    int64_t max_positive = (1 << (target_bits - 1)) - 1;  // For 4 bits: 7
    int64_t min_negative = -(1 << (target_bits - 1));     // For 4 bits: -8
    
    // reserve highest positive and lowest negative as escape codes
    int64_t positive_escape = max_positive;
    // int64_t negative_escape = min_negative;
    
    // max and min regular values
    int64_t maxRegularValue = baseValue + max_positive - 1;
    int64_t minRegularValue = baseValue + min_negative + 1;
    
    std::cout << "maxRegularValue:" << maxRegularValue << "\n";
    std::cout << "minRegularValue:" << minRegularValue << "\n";
    std::cout << "dataType:" << dataType << "\n";
    std::cout << "dataTypeInt: " << static_cast<int>(dataTypeInt) << "\n"; 

    outputFile.write(reinterpret_cast<char*>(&dataTypeInt), sizeof(dataTypeInt));


    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);


    if (dataType == "int8") {
        // origin value might be big, use int64_t
        int64_t valueOut;
        int8_t sub;
        while (inputFile >> valueOut) {
            // std::cout << "ori:" << valueOut << "\n";
            if ((minRegularValue <= valueOut) && (valueOut <= maxRegularValue) ){
                sub =  valueOut - baseValue;
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
    } else if (dataType == "int16") {
        int64_t valueOut;
        int16_t sub;
        while (inputFile >> valueOut) {
            // std::cout << "ori:" << valueOut << "\n";
            if ((minRegularValue <= valueOut) && (valueOut <= maxRegularValue) ){
                sub =  valueOut - baseValue;
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
        int64_t valueOut;
        int32_t sub;
        while (inputFile >> valueOut) {
            // std::cout << "ori:" << valueOut << "\n";
            if ((minRegularValue <= valueOut) && (valueOut <= maxRegularValue) ){
                sub =  valueOut - baseValue;
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
        int64_t valueOut;
        int64_t sub;
        while (inputFile >> valueOut) {
            // std::cout << "ori:" << valueOut << "\n";
            if ((minRegularValue <= valueOut) && (valueOut <= maxRegularValue) ){
                sub =  valueOut - baseValue;
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

void decompress_for(const std::string& dataTypeOri, const std::string& inputFilePath, const std::string& outputFilePath) {
    std::cout << "Start decompression. \n";
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
        int64_t value_escape;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            // std::cout << "value:" << static_cast<int>(value) << "\n";
            uint8_t target_bits = 8;
            int8_t max_positive = (1 << (target_bits - 1)) - 1;
            if (value!=max_positive){
                // can't determine int size after adding base value, set to int64
                int64_t valueAddBase = value + baseValue;
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
                // std::cout << "value:" << static_cast<int>(value_escape) << "\n";
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
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
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
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
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
                // std::cout << "value:" << static_cast<int>(valueAddBase) << "\n";
                outputFile << static_cast<int>(valueAddBase) << "\n";
            } else{
                inputFile.read(reinterpret_cast<char*>(&value_escape), sizeof(value_escape));
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