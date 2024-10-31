#include "compression_library.h"

// Template implementation for reading CSV and converting to numeric types
template<typename T>
std::vector<T> CSVReader::read_csv(const std::string& filename) {
    static_assert(std::is_arithmetic_v<T>, "Only numeric types supported");
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file");
    }

    std::vector<T> data;
    std::string line, value;

    // Read the first line (header), if any
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        
        // Assuming single column for simplicity
        if (std::getline(iss, value)) {
            try {
                if constexpr (std::is_integral_v<T>) {
                    data.push_back(std::stoll(value));
                } else {
                    data.push_back(std::stod(value));
                }
            } catch (const std::exception& e) {
                std::cerr << "Conversion error: " << e.what() << std::endl;
            }
        }
    }

    return data;
}

// Specialized template for string
std::vector<std::string> CSVReader::read_csv(const std::string& filename, std::true_type) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file");
    }

    std::vector<std::string> data;
    std::string line, value;

    // Read the first line (header), if any
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        
        // Assuming single column for simplicity
        if (std::getline(iss, value)) {
            data.push_back(value);
        }
    }

    return data;
}

int main() {
    try {
        // Example usage
        compress_file("input_int32.csv", 
                      "compressed_int32.bin", 
                      DataType::INT32, 
                      CompressionType::RLE);

        compress_file("input_string.csv", 
                      "compressed_string.bin", 
                      DataType::STRING, 
                      CompressionType::DIC);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}