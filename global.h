#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>


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