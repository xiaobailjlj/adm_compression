#include"global.h"  
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>
#include<sstream>
#include<chrono>
#include <bitset>
#include <variant>
#include <vector>
#include <cstdint>

template<typename T>
bool is_within_range_int8(std::vector<T>& values) {
    std::cout << "is_within_range_int8\n" << std::endl;
    for (const T& val : values) {
        if (val > 127 || val < -128) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool is_within_range_int16(std::vector<T>& values) {
    for (const T& val : values) {
        if (val > 32767 || val < -32768) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool is_within_range_int32(std::vector<T>& values) {
    for (const T& val : values) {
        if (val > 2147483647 || val < -2147483648) {
            return false;
        }
    }
    return true;
}

// indexInt: 1 for int8, 2 for int16, 3 for int32, 0 for other types
template<typename T>
void write_to_file(int originInt, int compressInt, std::vector<T>& values, std::ofstream& outputFile) {
    std::map<T, int> dictionary;
    std::vector<T> dicValues;
    int index = 0;
    for(const T& value : values) {
        // std::cout << "input:" << value << "\n" << std::endl;
        // std::cout << "bitset:" << std::bitset<8>(value) << "\n" << std::endl;
        if (dictionary.find(value) == dictionary.end()) {
            dicValues.push_back(value);
            dictionary[value] = index++;
        }
    }

    // write the compressed date type to output file
    outputFile.write(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    outputFile.write(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));

    // write the dictionary to output file
    outputFile.write(reinterpret_cast<char*>(&index), sizeof(index));

    for (int i = 0; i < dicValues.size(); i++) {
        // std::cout << "bitset:" << std::bitset<8>(dicValues[i]) << "\n" << std::endl;
        outputFile.write(reinterpret_cast<const char*>(&dicValues[i]), sizeof(dicValues[i]));
    }

   // write the compressed values to output file
    int valueCount = values.size();
    outputFile.write(reinterpret_cast<char*>(&valueCount), sizeof(valueCount));
    std::cout << "valueCount: " << valueCount << "\n" << std::endl;

    // compress index based on the range of values
    int indexInt = 0;
    if (index < 128 && index > -128) {
        indexInt = 1;
    } else if (index < 32768 && index > -32768) {
        indexInt = 2;
    } 
    std::cout << "indexInt: " << indexInt << "\n" << std::endl;
    outputFile.write(reinterpret_cast<char*>(&indexInt), sizeof(indexInt));

    for (const T& val : values) {
        if (indexInt==1) {
            int8_t index_int8 = static_cast<int8_t>(dictionary[val]);
            // std::cout << "bitset:" << std::bitset<8>(index_int8) << "\n" << std::endl;
            outputFile.write(reinterpret_cast<const char*>(&index_int8), sizeof(index_int8));
            continue;
        } else if (indexInt==2) {
            int16_t index_int16 = static_cast<int16_t>(dictionary[val]);
            outputFile.write(reinterpret_cast<char*>(&index_int16), sizeof(index_int16));
            continue;
        } 
        outputFile.write(reinterpret_cast<char*>(&dictionary[val]), sizeof(dictionary[val]));
    }
    
    outputFile.close();
}

// template<typename T>
// void compress_dic_impl_binary(const std::string& inputFilePath, const std::string& outputFilePath) {
//     std::ifstream inputFile(inputFilePath);
//     std::ofstream outputFile(outputFilePath, std::ios::binary);
//     // std::ofstream dicFile(dicFilePath, std::ios::binary);
//     if (!inputFile.is_open()) {
//         std::cerr << "Error: failed to open input file " << inputFilePath << std::endl;
//         return;
//     }
//     if (!outputFile.is_open()) {
//         std::cerr << "Error: failed to open output file " << outputFilePath << std::endl;
//         return;
//     }
//     // if (!dicFile.is_open()) {
//     //     std::cerr << "Error: failed to open dictionary file " << dicFilePath << std::endl;
//     //     return;
//     // }

//     std::vector<T> values;
//     T value;
//     while (inputFile >> value) {
//         // std::cout << "input:" << value << "\n" << std::endl;
//         values.push_back(value);
//     }

//     inputFile.close();

//     if(std::is_same<T, int16_t>::value) {
//         // std::cout << "if compressing int8_values\n" << std::endl;
//         if (is_within_range_int8(values)) {
//             std::vector<int8_t> int8_values;
//             for(const T& val : values){
//                 int8_values.push_back(static_cast<int8_t>(val));
//             }
//             std::cout << "compressing int8_values\n" << std::endl;
//             write_to_file<int8_t>(1, int8_values, outputFile);
//             return;
//         }
//     } else if (std::is_same<T, int32_t>::value) {
//             if (is_within_range_int8(values)) {
//                std::vector<int8_t> int8_values;
//                for(const T& val : values){
//                     int8_values.push_back(static_cast<int8_t>(val));
//                }
//                std::cout << "compressing int8_values\n" << std::endl;
//                 write_to_file<int8_t>(1, int8_values, outputFile);
//                 return;
//             }else if (is_within_range_int16(values)) {
//                std::vector<int16_t> int16_values;
//                for(const T& val : values){
//                     int16_values.push_back(static_cast<int16_t>(val));
//                }
//                std::cout << "compressing int16_values\n" << std::endl;
//                 write_to_file<int16_t>(2, int16_values, outputFile);
//                 return;
//             }
//     } else if (std::is_same<T, int64_t>::value) {
//             if (is_within_range_int8(values)) {
//                std::vector<int8_t> int8_values;
//                for(const T& val : values){
//                     int8_values.push_back(static_cast<int8_t>(val));
//                }
//                std::cout << "compressing int8_values\n" << std::endl;
//                 write_to_file<int8_t>(1, int8_values, outputFile);
//                 return;
//             }else if (is_within_range_int16(values)) {
//                std::vector<int16_t> int16_values;
//                for(const T& val : values){
//                     int16_values.push_back(static_cast<int16_t>(val));
//                }
//                std::cout << "compressing int16_values\n" << std::endl;
//                 write_to_file<int16_t>(2, int16_values, outputFile);
//                 return;
//             }else if (is_within_range_int32(values)) {
//                std::vector<int32_t> int32_values;
//                for(const T& val : values){
//                     int32_values.push_back(static_cast<int32_t>(val));
//                }
//                std::cout << "compressing int32_values\n" << std::endl;
//                 write_to_file<int32_t>(3, int32_values, outputFile);
//                 return;
//             }
//     }

//     write_to_file<T>(0, values, outputFile);
// }


void compress_dic_impl_binary_int8(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    // std::ofstream dicFile(dicFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: failed to open input file " << inputFilePath << std::endl;
        return;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: failed to open output file " << outputFilePath << std::endl;
        return;
    }
    // if (!dicFile.is_open()) {
    //     std::cerr << "Error: failed to open dictionary file " << dicFilePath << std::endl;
    //     return;
    // }

    std::vector<int> values;
    int value;
    while (inputFile >> value) {
        // std::cout << "input:" << value << "\n" << std::endl;
        values.push_back(value);
    }

    inputFile.close();

    std::vector<int8_t> int8_values;
    for(const int& val : values){
        int8_values.push_back(static_cast<int8_t>(val));
    }

    write_to_file<int8_t>(1, 1, int8_values, outputFile);

}

void compress_dic_impl_int16(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    // std::ofstream dicFile(dicFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: failed to open input file " << inputFilePath << std::endl;
        return;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: failed to open output file " << outputFilePath << std::endl;
        return;
    }

    std::vector<int16_t> values;
    int16_t value;
    while (inputFile >> value) {
        // std::cout << "input:" << value << "\n" << std::endl;
        values.push_back(value);
    }

    inputFile.close();


    write_to_file<int16_t>(2, 2, values, outputFile);
}

void compress_dic_impl_int32(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    // std::ofstream dicFile(dicFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: failed to open input file " << inputFilePath << std::endl;
        return;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: failed to open output file " << outputFilePath << std::endl;
        return;
    }

    std::vector<int32_t> values;
    int32_t value;
    while (inputFile >> value) {
        // std::cout << "input:" << value << "\n" << std::endl;
        values.push_back(value);
    }

    inputFile.close();

    // if (is_within_range_int8(values)) {
    //     std::vector<int8_t> int8_values;
    //     for(const int32_t& val : values){
    //         int8_values.push_back(static_cast<int8_t>(val));
    //     }
    //     std::cout << "compressing int8_values\n" << std::endl;
    //     write_to_file<int8_t>(3, 1, int8_values, outputFile);
    //     return;
    // }else if (is_within_range_int16(values)) {
    //     std::vector<int16_t> int16_values;
    //     for(const int32_t& val : values){
    //         int16_values.push_back(static_cast<int16_t>(val));
    //     }
    //     std::cout << "compressing int16_values\n" << std::endl;
    //     write_to_file<int16_t>(3,2, int16_values, outputFile);
    //     return;
    // }

    write_to_file<int32_t>(3, 3, values, outputFile);
}

void compress_dic_impl_int64(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    // std::ofstream dicFile(dicFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: failed to open input file " << inputFilePath << std::endl;
        return;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: failed to open output file " << outputFilePath << std::endl;
        return;
    }

    std::vector<int64_t> values;
    int64_t value;
    while (inputFile >> value) {
        // std::cout << "input:" << value << "\n" << std::endl;
        values.push_back(value);
    }

    inputFile.close();

    write_to_file<int64_t>(4, 4, values, outputFile);
}



void compress_dic_impl_string(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath);
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    // std::ofstream dicFile(dicFilePath, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Error: failed to open input file " << inputFilePath << std::endl;
        return;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: failed to open output file " << outputFilePath << std::endl;
        return;
    }
    // if (!dicFile.is_open()) {
    //     std::cerr << "Error: failed to open dictionary file " << dicFilePath << std::endl;
    //     return;
    // }

    std::vector<std::string> values;
    std::string value;
    std::string line;
    while (std::getline(inputFile, line)) {
        // std::cout << "input:" << line << "\n" << std::endl;
        values.push_back(line);
    }

    inputFile.close();

    // write_to_file<std::string>(5,5, values, outputFile);

    std::map<std::string, int> dictionary;
    std::vector<std::string> dicValues;
    int index = 0;
    for(const std::string& value : values) {
        // std::cout << "input:" << value << "\n" << std::endl;
        // std::cout << "bitset:" << std::bitset<8>(value) << "\n" << std::endl;
        if (dictionary.find(value) == dictionary.end()) {
            dicValues.push_back(value);
            dictionary[value] = index++;
        }
    }

    int originInt = 5;
    int compressInt = 5;
    // write the compressed date type to output file
    outputFile.write(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    outputFile.write(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));

    // write the dictionary to output file
    outputFile.write(reinterpret_cast<char*>(&index), sizeof(index));
    std::cout << "dictSize:" << index << "\n" << std::endl;

    for (int i = 0; i < dicValues.size(); i++) {
        // std::cout << "bitset:" << std::bitset<8>(dicValues[i]) << "\n" << std::endl;

        int size = sizeof(dicValues[i]);
        // outputFile.write(reinterpret_cast<char*>(&size), sizeof(size));
        outputFile.write(reinterpret_cast<char*>(&dicValues[i]), sizeof(dicValues[i]));
        // std::cout << "size:" << size << "\n" << std::endl;
        // std::cout << "value:" << dicValues[i] << "\n" << std::endl;
    }

   // write the compressed values to output file
    int valueCount = values.size();
    outputFile.write(reinterpret_cast<char*>(&valueCount), sizeof(valueCount));
    std::cout << "valueCount: " << valueCount << "\n" << std::endl;

    // compress index based on the range of values
    int indexInt = 0;
    if (index < 128 && index > -128) {
        indexInt = 1;
    } else if (index < 32768 && index > -32768) {
        indexInt = 2;
    } 
    std::cout << "indexInt: " << indexInt << "\n" << std::endl;
    outputFile.write(reinterpret_cast<char*>(&indexInt), sizeof(indexInt));

    for (const std::string& val : values) {
        if (indexInt==1) {
            int8_t index_int8 = static_cast<int8_t>(dictionary[val]);
            // std::cout << "bitset:" << std::bitset<8>(index_int8) << "\n" << std::endl;
            outputFile.write(reinterpret_cast<const char*>(&index_int8), sizeof(index_int8));
            continue;
        } else if (indexInt==2) {
            int16_t index_int16 = static_cast<int16_t>(dictionary[val]);
            outputFile.write(reinterpret_cast<char*>(&index_int16), sizeof(index_int16));
            continue;
        } 
        outputFile.write(reinterpret_cast<char*>(&dictionary[val]), sizeof(dictionary[val]));
    }
    
    outputFile.close();
}


// template<typename T>
// void decompress_dic_impl(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& dicFilePath) {
//     std::ifstream inputFile(inputFilePath, std::ios::binary);
//     std::ifstream dicFile(dicFilePath);
//     std::ofstream outputFile(outputFilePath);
    
//     if (!inputFile.is_open()) {
//         throw std::runtime_error("Cannot open input file: " + inputFilePath);
//     }
//     if (!outputFile.is_open()) {
//         throw std::runtime_error("Cannot open output file: " + outputFilePath);
//     }
//     if (!dicFile.is_open()) {
//         throw std::runtime_error("Cannot open dictionary file: " + dicFilePath);
//     }

    
//     // 1. Read dictionary size
//     int dictSize;
//     dicFile >> dictSize;

//     // 2. Read dictionary
//     std::vector<T> dictionary(dictSize);
//     for (int i = 0; i < dictSize; i++) {
//         dicFile >> dictionary[i];
//         // std::cout << "dictionary: " << dictionary[i] << "\n" << std::endl;
//     }

//     // // 3. Read number of values
//     int valueCount;
//     inputFile >> valueCount;

//     // 4. Read and decode values
//     for (int i = 0; i < valueCount; i++) {
//         int index;
//         inputFile >> index;
//         if (index < dictionary.size()) {
//             // std::cout << "data: " << dictionary[index] << "\n" << std::endl;
//             outputFile << dictionary[index] << "\n";
//         }
//     }

//     inputFile.close();
//     outputFile.close();
//     dicFile.close();
// }


// void decompress_dic_int8(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& dicFilePath) {
//     std::ifstream inputFile(inputFilePath, std::ios::binary);
//     std::ifstream dicFile(dicFilePath);
//     std::ofstream outputFile(outputFilePath);
    
//     if (!inputFile.is_open()) {
//         throw std::runtime_error("Cannot open input file: " + inputFilePath);
//     }
//     if (!outputFile.is_open()) {
//         throw std::runtime_error("Cannot open output file: " + outputFilePath);
//     }
//     if (!dicFile.is_open()) {
//         throw std::runtime_error("Cannot open dictionary file: " + dicFilePath);
//     }
    
//     // 1. Read dictionary size
//     int dictSize;
//     dicFile >> dictSize;

//     // 2. Read dictionary
//     std::vector<int> dictionary(dictSize);

//     for (int i = 0; i < dictSize; i++) {
    
//         dicFile >> dictionary[i];
//         // std::cout << "dictionary: " << dictionary[i] << "\n" << std::endl;
//     }

//     // // 3. Read number of values
//     int valueCount;
//     inputFile >> valueCount;

//     // 4. Read and decode values
//     for (int i = 0; i < valueCount; i++) {
//         int index;
//         inputFile >> index;
//         if (index < dictionary.size()) {
//             // std::cout << "data: " << dictionary[index] << "\n" << std::endl;
//             outputFile << dictionary[index] << "\n";
//         }
//     }

//     inputFile.close();
//     outputFile.close();
//     dicFile.close();
// }


// template<typename T>
// void decompress_dic_impl_binary(const std::string& inputFilePath, const std::string& outputFilePath) {
//     std::ifstream inputFile(inputFilePath, std::ios::binary);
//     std::ofstream outputFile(outputFilePath);
    
//     if (!inputFile.is_open()) {
//         throw std::runtime_error("Cannot open input file: " + inputFilePath);
//     }
//     if (!outputFile.is_open()) {
//         throw std::runtime_error("Cannot open output file: " + outputFilePath);
//     }

//     // 1. Read dictionary size
//     int dictSize;
//     inputFile.read(reinterpret_cast<char*>(&dictSize), sizeof(dictSize));
//     std::cout << "dictSize:" << dictSize << "\n" << std::endl;

//     int flagInt;
//     inputFile.read(reinterpret_cast<char*>(&flagInt), sizeof(flagInt));
//     std::cout << "flagInt:" << flagInt << "\n" << std::endl;

//     // 2. Read dictionary
//     if (flagInt==1) {
//         std::vector<int8_t> dictionary(dictSize);
//         for (int i = 0; i < dictSize; i++) {
//             int8_t value;
//             inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
//             dictionary[i] = value;
//             // std::cout << "dictionary:bitset:" << std::bitset<16>(value) << "\n" << std::endl;
//         }
//         int valueCount;
//         inputFile.read(reinterpret_cast<char*>(&valueCount), sizeof(valueCount));
//         std::cout << "valueCount:" << valueCount << "\n" << std::endl;

//         int flag;
//         inputFile.read(reinterpret_cast<char*>(&flag), sizeof(flag));
//         std::cout << "flag:" << flag << "\n" << std::endl;

//         // 4. Read and decode values
//         for (int i = 0; i < valueCount; i++) {
//             if (flag==1) {      
//                 int8_t index_int8;
//                 inputFile.read(reinterpret_cast<char*>(&index_int8), sizeof(index_int8));   
//                 if (index_int8 < dictionary.size())  {
//                     // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index_int8]) << "\n" << std::endl;        
//                     outputFile << static_cast<int>(dictionary[index_int8]) << "\n";
//                 }
//             } else if(flag==2) {
//                 int16_t index_int16;    
//                 inputFile.read(reinterpret_cast<char*>(&index_int16), sizeof(index_int16));
//                 if (index_int16 < dictionary.size())    {       
//                     outputFile << dictionary[index_int16] << "\n";
//                 }
//             } else if(flag==3) {
//                 int32_t index_int32;
//                 inputFile.read(reinterpret_cast<char*>(&index_int32), sizeof(index_int32));
//                 if (index_int32 < dictionary.size())    {
//                     // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index_int32]) << "\n" << std::endl;        
//                     outputFile << dictionary[index_int32] << "\n";
//                 }
//             }else {
//                 int index;
//                 inputFile.read(reinterpret_cast<char*>(&index), sizeof(index));
//                 if (index < dictionary.size()) {
//                     // std::cout << "data: " << dictionary[index] << "\n" << std::endl;
//                     outputFile << dictionary[index] << "\n";
//                 }
//             }
//         }

//         inputFile.close();
//         outputFile.close();
//         return;
//     } else if(flagInt==2) {
//         std::vector<int16_t> dictionary(dictSize);
//         for (int i = 0; i < dictSize; i++) {
//             int16_t value;
//             inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
//             dictionary[i] = value;
//             // std::cout << "dictionary:bitset:" << std::bitset<16>(value) << "\n" << std::endl;
//         }
//         decompress_write_file<int16_t>(dictionary, inputFile, outputFile);
//         return;
//     } else if(flagInt==3) { 
//         std::vector<int32_t> dictionary(dictSize);
//         for (int i = 0; i < dictSize; i++) {
//             int32_t value;
//             inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
//             dictionary[i] = value;
//             // std::cout << "dictionary:bitset:" << std::bitset<16>(value) << "\n" << std::endl;
//         }
//         decompress_write_file<int32_t>(dictionary, inputFile, outputFile);
//         return;
//     }
    
//     std::vector<T> dictionary(dictSize);
//     for (int i = 0; i < dictSize; i++) {
//         T value;
//         inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
//         dictionary[i] = value;
//     }
//     decompress_write_file<T>(dictionary, inputFile, outputFile)

// }

void decompress_dic_impl_binary_int8(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    // std::ifstream dicFile(dicFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);
    
    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }
    // if (!dicFile.is_open()) {
    //     throw std::runtime_error("Cannot open dictionary file: " + dicFilePath);
    // }

    int originInt;
    inputFile.read(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    std::cout << "originInt:" << originInt << "\n" << std::endl;

    int compressInt;
    inputFile.read(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));
    std::cout << "compressInt:" << compressInt << "\n" << std::endl;
    
    // 1. Read dictionary size
    int dictSize;
    inputFile.read(reinterpret_cast<char*>(&dictSize), sizeof(dictSize));

    // 2. Read dictionary
    std::vector<int8_t> dictionary(dictSize);
    for (int i = 0; i < dictSize; i++) {
        int8_t value;
        inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
        dictionary[i] = value;
        // std::cout << "dictionary:bitset:" << std::bitset<8>(value) << "\n" << std::endl;
    }

    // // 3. Read number of values
    int valueCount;
    inputFile.read(reinterpret_cast<char*>(&valueCount), sizeof(valueCount));
    std::cout << "valueCount:" << valueCount << "\n" << std::endl;

    int indexInt;
    inputFile.read(reinterpret_cast<char*>(&indexInt), sizeof(indexInt));
    std::cout << "indexInt:" << indexInt << "\n" << std::endl;

    // 4. Read and decode values
    for (int i = 0; i < valueCount; i++) {
        if (indexInt==1) {
            int8_t index_int8;
            inputFile.read(reinterpret_cast<char*>(&index_int8), sizeof(index_int8));
            if (index_int8 < dictionary.size())    {
                // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index_int8]) << "\n" << std::endl;        
                outputFile << static_cast<int>(dictionary[index_int8]) << "\n";
            }
            continue;
        } else if (indexInt==2) {
            int16_t index_int16;
            inputFile.read(reinterpret_cast<char*>(&index_int16), sizeof(index_int16));
            if (index_int16 < dictionary.size())    {
                // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index_int16]) << "\n" << std::endl;        
                outputFile << static_cast<int>(dictionary[index_int16]) << "\n";
            }
            continue;
        } 
        int index;
        inputFile.read(reinterpret_cast<char*>(&index), sizeof(index));
        if (index < dictionary.size()) {
            // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index]) << "\n" << std::endl;
            outputFile << static_cast<int>(dictionary[index]) << "\n";
        }
    }

    inputFile.close();
    outputFile.close();
    // dicFile.close();
}

template<typename T>
void decompress_write_file(std::vector<T>& dictionary, std::ifstream& inputFile, std::ofstream& outputFile) {
    // 3. Read number of values
    int valueCount;
    inputFile.read(reinterpret_cast<char*>(&valueCount), sizeof(valueCount));
    std::cout << "valueCount:" << valueCount << "\n" << std::endl;

    int indexInt;
    inputFile.read(reinterpret_cast<char*>(&indexInt), sizeof(indexInt));
    std::cout << "indexInt:" << indexInt << "\n" << std::endl;

    // 4. Read and decode values
    for (int i = 0; i < valueCount; i++) {
        if (indexInt==1) {
            int8_t index_int8;
            inputFile.read(reinterpret_cast<char*>(&index_int8), sizeof(index_int8));
            if (index_int8 < dictionary.size())    {
                // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index_int8]) << "\n" << std::endl;        
                outputFile << dictionary[index_int8] << "\n";
            }
            continue;
        } else if (indexInt==2) {
            int16_t index_int16;
            inputFile.read(reinterpret_cast<char*>(&index_int16), sizeof(index_int16));
            if (index_int16 < dictionary.size())    {
                // std::cout << "data:bitset:" << std::bitset<16>(dictionary[index_int16]) << "\n" << std::endl;        
                outputFile << dictionary[index_int16] << "\n";
            }
            continue;
        } 
        int index;
        inputFile.read(reinterpret_cast<char*>(&index), sizeof(index));
        if (index < dictionary.size()) {
            // std::cout << "data: " << dictionary[index] << "\n" << std::endl;
            outputFile << dictionary[index] << "\n";
        }
    }

    inputFile.close();
    outputFile.close();
}


void decompress_dic_impl_binary_int16(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);
    
    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }


    int originInt;
    inputFile.read(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    std::cout << "originInt:" << originInt << "\n" << std::endl;

    int compressInt;
    inputFile.read(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));
    std::cout << "compressInt:" << compressInt << "\n" << std::endl;

    // 1. Read dictionary size
    int dictSize;
    inputFile.read(reinterpret_cast<char*>(&dictSize), sizeof(dictSize));
    std::cout << "dictSize:" << dictSize << "\n" << std::endl;


    // 2. Read dictionary
    // if (originInt==2 && compressInt==1) {
    //     std::vector<int8_t> dictionary(dictSize);
    //     for (int i = 0; i < dictSize; i++) {
    //         int16_t value;
    //         inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
    //         dictionary[i] = value;
    //     }

    //     // 3. Read number of values
    //     int valueCount;
    //     inputFile.read(reinterpret_cast<char*>(&valueCount), sizeof(valueCount));
    //     std::cout << "valueCount:" << valueCount << "\n" << std::endl;

    //     int flag;
    //     inputFile.read(reinterpret_cast<char*>(&flag), sizeof(flag));
    //     std::cout << "flag:" << flag << "\n" << std::endl;

    //     // 4. Read and decode values
    //     for (int i = 0; i < valueCount; i++) {
    //         int index;
    //         inputFile.read(reinterpret_cast<char*>(&index), sizeof(index));
    //         if (index < dictionary.size()) {
    //             // std::cout << "data: " << dictionary[index] << "\n" << std::endl;
    //             outputFile << dictionary[index] << "\n";
    //         }
    //     }
    //     inputFile.close();
    //     outputFile.close();
    //     return;
    // } 
    
    std::vector<int16_t> dictionary(dictSize);
    for (int i = 0; i < dictSize; i++) {
        int16_t value;
        inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
        dictionary[i] = value;
    }
    
    decompress_write_file<int16_t>(dictionary, inputFile, outputFile);

}

void decompress_dic_impl_binary_int32(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);
    
    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    int originInt;
    inputFile.read(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    std::cout << "originInt:" << originInt << "\n" << std::endl;

    int compressInt;
    inputFile.read(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));
    std::cout << "compressInt:" << compressInt << "\n" << std::endl;

    // 1. Read dictionary size
    int dictSize;
    inputFile.read(reinterpret_cast<char*>(&dictSize), sizeof(dictSize));
    std::cout << "dictSize:" << dictSize << "\n" << std::endl;


    // 2. Read dictionary
    std::vector<int32_t> dictionary(dictSize);
    for (int i = 0; i < dictSize; i++) {
        int32_t value;
        inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
        dictionary[i] = value;
    }
    decompress_write_file<int32_t>(dictionary, inputFile, outputFile);

}

void decompress_dic_impl_binary_int64(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);
    
    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    int originInt;
    inputFile.read(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    std::cout << "originInt:" << originInt << "\n" << std::endl;

    int compressInt;
    inputFile.read(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));
    std::cout << "compressInt:" << compressInt << "\n" << std::endl;

    // 1. Read dictionary size
    int dictSize;
    inputFile.read(reinterpret_cast<char*>(&dictSize), sizeof(dictSize));
    std::cout << "dictSize:" << dictSize << "\n" << std::endl;


    // 2. Read dictionary
    std::vector<int64_t> dictionary(dictSize);
    for (int i = 0; i < dictSize; i++) {
        int64_t value;
        inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
        dictionary[i] = value;
    }
    decompress_write_file<int64_t>(dictionary, inputFile, outputFile);
}

void decompress_dic_impl_binary_string(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    std::ofstream outputFile(outputFilePath);
    
    if (!inputFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputFilePath);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputFilePath);
    }

    int originInt;
    inputFile.read(reinterpret_cast<char*>(&originInt), sizeof(originInt));
    std::cout << "originInt:" << originInt << "\n" << std::endl;

    int compressInt;
    inputFile.read(reinterpret_cast<char*>(&compressInt), sizeof(compressInt));
    std::cout << "compressInt:" << compressInt << "\n" << std::endl;

    // 1. Read dictionary size
    int dictSize;
    inputFile.read(reinterpret_cast<char*>(&dictSize), sizeof(dictSize));
    std::cout << "dictSize:" << dictSize << "\n" << std::endl;


    // 2. Read dictionary
    std::vector<std::string> dictionary(dictSize);
    for (int i = 0; i < dictSize; i++) {
        // int valueSize;
        // inputFile.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        // std::cout << "dictionary:size:" << valueSize << "\n" << std::endl;
        std::string value;
        inputFile.read(reinterpret_cast<char*>(&value), sizeof(value));
        // std::cout << "dictionary: " << value << "\n" << std::endl;
        dictionary[i] = value;
    }
    decompress_write_file<std::string>(dictionary, inputFile, outputFile);
}


void compress_dic(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath) {
    auto start = std::chrono::high_resolution_clock::now();
    if (dataType == "int8") {
        // compress_dic_impl_int8(inputFilePath, outputFilePath, dicFilePath);
        compress_dic_impl_binary_int8(inputFilePath, outputFilePath);
    } else if (dataType == "int16") {
        // compress_dic_impl<int16_t>(inputFilePath, outputFilePath, dicFilePath);
        compress_dic_impl_int16(inputFilePath, outputFilePath);
    } else if (dataType == "int32") {
        compress_dic_impl_int32(inputFilePath, outputFilePath);
    } else if (dataType == "int64") {
        compress_dic_impl_int64(inputFilePath, outputFilePath);
    } else if (dataType == "string"){
        compress_dic_impl_string(inputFilePath, outputFilePath);
    }else {
        throw std::invalid_argument("Unsupported data type for dictionary encoding");
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start)/1000000.0;
    std::cout << "Time taken for dictionary encoding: " << duration.count() << " seconds" << std::endl;
}

void decompress_dic(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath) {
    auto start = std::chrono::high_resolution_clock::now();
    if (dataType == "int8") {
        // decompress_dic_int8(inputFilePath, outputFilePath, dicFilePath);
        decompress_dic_impl_binary_int8(inputFilePath, outputFilePath);
    } else if (dataType == "int16") {
        decompress_dic_impl_binary_int16(inputFilePath, outputFilePath);
    } else if (dataType == "int32") {
        decompress_dic_impl_binary_int32(inputFilePath, outputFilePath);
    } else if (dataType == "int64") {
        decompress_dic_impl_binary_int64(inputFilePath, outputFilePath);
    } else if (dataType == "string"){
        decompress_dic_impl_binary_string(inputFilePath, outputFilePath);
    }else {
        throw std::invalid_argument("Unsupported data type for dictionary encoding");
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start)/1000000.0;
    std::cout << "Time taken for dictionary decoding: " << duration.count() << " seconds" << std::endl;
}



