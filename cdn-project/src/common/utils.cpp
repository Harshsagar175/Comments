#include "common.h"
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <openssl/sha.h>

namespace cdn {

double Utils::calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    // Haversine formula for calculating distance between two points on Earth
    const double R = 6371000; // Earth's radius in meters
    
    double lat1_rad = lat1 * M_PI / 180.0;
    double lat2_rad = lat2 * M_PI / 180.0;
    double delta_lat = (lat2 - lat1) * M_PI / 180.0;
    double delta_lon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = sin(delta_lat / 2) * sin(delta_lat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2) * sin(delta_lon / 2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return R * c; // Distance in meters
}

std::string Utils::generate_file_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

std::string Utils::calculate_file_hash(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    if (file.gcount() > 0) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return ss.str();
}

std::string Utils::get_client_ip(const http_request& request) {
    auto headers = request.headers();
    
    // Check for X-Forwarded-For header first (proxy)
    auto xff = headers.find("X-Forwarded-For");
    if (xff != headers.end()) {
        std::string forwarded = xff->second;
        size_t comma = forwarded.find(',');
        if (comma != std::string::npos) {
            return forwarded.substr(0, comma);
        }
        return forwarded;
    }
    
    // Check X-Real-IP header
    auto real_ip = headers.find("X-Real-IP");
    if (real_ip != headers.end()) {
        return real_ip->second;
    }
    
    // Fall back to remote address
    return request.remote_address();
}

json::value Utils::create_error_response(const std::string& error_msg) {
    json::value response = json::value::object();
    response["success"] = json::value::boolean(false);
    response["error"] = json::value::string(error_msg);
    response["timestamp"] = json::value::string(
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count())
    );
    return response;
}

json::value Utils::create_success_response(const json::value& data) {
    json::value response = json::value::object();
    response["success"] = json::value::boolean(true);
    response["data"] = data;
    response["timestamp"] = json::value::string(
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count())
    );
    return response;
}

bool Utils::create_directory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::filesystem::filesystem_error& e) {
        Logger::error("Utils", "Failed to create directory " + path + ": " + e.what());
        return false;
    }
}

std::vector<std::string> Utils::list_files_in_directory(const std::string& directory) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        Logger::error("Utils", "Failed to list files in directory " + directory + ": " + e.what());
    }
    
    return files;
}

} // namespace cdn