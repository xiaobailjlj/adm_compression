#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>

//  five compression methods:
//  1. binary compression
//  2. run-length encoding
//  3. dictionary encoding
//  4. frame-of-reference encoding
//  5. differential encoding

//  five data types:
//  1. int8
//  2. int16
//  3. int32
//  4. int64
//  5. string

void compress_bin(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void decompress_bin(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void compress_rle(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void decompress_rle(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void compress_dic(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void decompress_dic(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void compress_for(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void decompress_for(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void compress_dif(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void decompress_dif(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
#endif