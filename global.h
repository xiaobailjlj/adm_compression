#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>


void compress_bin(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);
void decompress_bin(const std::string& dataType, const std::string& inputFilePath, const std::string& outputFilePath);


#endif