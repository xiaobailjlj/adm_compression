#ifndef COMPRESSION_LIBRARY_H
#define COMPRESSION_LIBRARY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

enum class CompressionType {
    BIN,    // Uncompressed binary
    RLE,    // Run-length encoding
    DIC,    // Dictionary encoding
    FOR,    // Frame of reference encoding
    DIF     // Differential encoding
};

enum class DataType {
    INT8,
    INT16,
    INT32,
    INT64,
    STRING
};

class Compressor {
private:
    // Helper function to get size of data types
    template<typename T>
    static constexpr size_t get_type_size() {
        if constexpr (std::is_same_v<T, int8_t>) return 1;
        else if constexpr (std::is_same_v<T, int16_t>) return 2;
        else if constexpr (std::is_same_v<T, int32_t>) return 4;
        else if constexpr (std::is_same_v<T, int64_t>) return 8;
        else return 0;
    }

public:
    // Binary Encoding (Uncompressed)
    template<typename T>
    static void compress_bin(const std::vector<T>& data, std::ofstream& out) {
        static_assert(std::is_integral_v<T>, "Only integer types supported");
        for (const auto& val : data) {
            out.write(reinterpret_cast<const char*>(&val), sizeof(T));
        }
    }

    // Run-Length Encoding
    template<typename T>
    static void compress_rle(const std::vector<T>& data, std::ofstream& out) {
        if (data.empty()) return;

        T current_val = data[0];
        size_t count = 1;

        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i] == current_val) {
                count++;
            } else {
                // Write count and value
                out.write(reinterpret_cast<const char*>(&count), sizeof(size_t));
                out.write(reinterpret_cast<const char*>(&current_val), sizeof(T));
                
                current_val = data[i];
                count = 1;
            }
        }

        // Write last run
        out.write(reinterpret_cast<const char*>(&count), sizeof(size_t));
        out.write(reinterpret_cast<const char*>(&current_val), sizeof(T));
    }

    // Dictionary Encoding
    template<typename T>
    static void compress_dic(const std::vector<T>& data, std::ofstream& out) {
        std::unordered_map<T, uint32_t> dictionary;
        std::vector<uint32_t> encoded_data;

        // Build dictionary and encode
        uint32_t dict_index = 0;
        for (const auto& val : data) {
            if (dictionary.find(val) == dictionary.end()) {
                dictionary[val] = dict_index++;
            }
            encoded_data.push_back(dictionary[val]);
        }

        // Write dictionary size
        size_t dict_size = dictionary.size();
        out.write(reinterpret_cast<const char*>(&dict_size), sizeof(size_t));

        // Write dictionary entries
        for (const auto& [key, _] : dictionary) {
            out.write(reinterpret_cast<const char*>(&key), sizeof(T));
        }

        // Write encoded data
        for (const auto& encoded : encoded_data) {
            out.write(reinterpret_cast<const char*>(&encoded), sizeof(uint32_t));
        }
    }

    // Frame of Reference Encoding (for integer types)
    template<typename T>
    static void compress_for(const std::vector<T>& data, std::ofstream& out) {
        static_assert(std::is_integral_v<T>, "Only integer types supported");
        if (data.empty()) return;

        // Find base value (minimum)
        T base = *std::min_element(data.begin(), data.end());
        out.write(reinterpret_cast<const char*>(&base), sizeof(T));

        // Write relative values
        for (const auto& val : data) {
            T relative_val = val - base;
            out.write(reinterpret_cast<const char*>(&relative_val), sizeof(T));
        }
    }

    // Differential Encoding (for integer types)
    template<typename T>
    static void compress_dif(const std::vector<T>& data, std::ofstream& out) {
        static_assert(std::is_integral_v<T>, "Only integer types supported");
        if (data.empty()) return;

        // First value written as is
        out.write(reinterpret_cast<const char*>(&data[0]), sizeof(T));

        // Write differences
        for (size_t i = 1; i < data.size(); ++i) {
            T diff = data[i] - data[i-1];
            out.write(reinterpret_cast<const char*>(&diff), sizeof(T));
        }
    }

    // String compression (dictionary-based)
    static void compress_string(const std::vector<std::string>& data, std::ofstream& out) {
        std::unordered_map<std::string, uint32_t> dictionary;
        std::vector<uint32_t> encoded_data;

        // Build dictionary and encode
        uint32_t dict_index = 0;
        for (const auto& val : data) {
            if (dictionary.find(val) == dictionary.end()) {
                dictionary[val] = dict_index++;
            }
            encoded_data.push_back(dictionary[val]);
        }

        // Write dictionary size
        size_t dict_size = dictionary.size();
        out.write(reinterpret_cast<const char*>(&dict_size), sizeof(size_t));

        // Write dictionary entries (with string lengths)
        for (const auto& [key, _] : dictionary) {
            size_t len = key.length();
            out.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
            out.write(key.c_str(), len);
        }

        // Write encoded data
        for (const auto& encoded : encoded_data) {
            out.write(reinterpret_cast<const char*>(&encoded), sizeof(uint32_t));
        }
    }

    // Compression router
    template<typename T>
    static void compress(const std::vector<T>& data, CompressionType type, std::ofstream& out) {
        switch (type) {
            case CompressionType::BIN: 
                compress_bin(data, out); 
                break;
            case CompressionType::RLE: 
                compress_rle(data, out); 
                break;
            case CompressionType::DIC: 
                compress_dic(data, out); 
                break;
            case CompressionType::FOR: 
                compress_for(data, out); 
                break;
            case CompressionType::DIF: 
                compress_dif(data, out); 
                break;
            default:
                throw std::runtime_error("Invalid compression type");
        }
    }

    // Decompression methods will be similar, mirroring the compression techniques
    // Omitted for brevity, but they would be symmetrical to the compression methods
};

// CSV Reader utility
class CSVReader {
public:
    // Read CSV and convert to appropriate type
    template<typename T>
    static std::vector<T> read_csv(const std::string& filename);

    // Specialized template for string
    static std::vector<std::string> read_csv(const std::string& filename, std::true_type);
};

// Main compression function
void compress_file(const std::string& input_csv, 
                   const std::string& output_binary, 
                   DataType data_type, 
                   CompressionType compression_type) {
    std::ofstream out_file(output_binary, std::ios::binary);
    if (!out_file) {
        throw std::runtime_error("Cannot open output file");
    }

    // Write metadata
    uint8_t type_code = static_cast<uint8_t>(data_type);
    uint8_t comp_code = static_cast<uint8_t>(compression_type);
    out_file.write(reinterpret_cast<const char*>(&type_code), sizeof(uint8_t));
    out_file.write(reinterpret_cast<const char*>(&comp_code), sizeof(uint8_t));

    // Compression based on data type
    switch (data_type) {
        case DataType::INT8: {
            auto data = CSVReader::read_csv<int8_t>(input_csv);
            Compressor::compress(data, compression_type, out_file);
            break;
        }
        case DataType::INT16: {
            auto data = CSVReader::read_csv<int16_t>(input_csv);
            Compressor::compress(data, compression_type, out_file);
            break;
        }
        case DataType::INT32: {
            auto data = CSVReader::read_csv<int32_t>(input_csv);
            Compressor::compress(data, compression_type, out_file);
            break;
        }
        case DataType::INT64: {
            auto data = CSVReader::read_csv<int64_t>(input_csv);
            Compressor::compress(data, compression_type, out_file);
            break;
        }
        case DataType::STRING: {
            auto data = CSVReader::read_csv(input_csv, std::true_type{});
            Compressor::compress_string(data, out_file);
            break;
        }
    }

    out_file.close();
}

#endif // COMPRESSION_LIBRARY_H