#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <cpprest/http_listener.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace web::http::client;

namespace cdn {

// Configuration constants
const int DEFAULT_PORT = 8080;
const size_t MAX_CACHE_SIZE = 1024 * 1024 * 1024; // 1GB
const int CACHE_TTL_SECONDS = 3600; // 1 hour
const int PREFETCH_BATCH_SIZE = 10;

// Forward declarations
class FileInfo;
class CDNNode;
class MetaServer;

// File metadata structure
struct FileMetadata {
    std::string file_id;
    std::string filename;
    std::string file_hash;
    size_t file_size;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point modified_at;
    std::vector<std::string> cdn_node_ids; // CDN nodes that have this file
    
    json::value to_json() const;
    static FileMetadata from_json(const json::value& val);
};

// CDN Node information
struct CDNNodeInfo {
    std::string node_id;
    std::string ip_address;
    int port;
    double latitude;
    double longitude;
    size_t cache_capacity;
    size_t cache_usage;
    std::chrono::system_clock::time_point last_heartbeat;
    bool is_active;
    
    json::value to_json() const;
    static CDNNodeInfo from_json(const json::value& val);
};

// Cache entry structure
struct CacheEntry {
    std::string file_id;
    std::string file_path;
    size_t file_size;
    std::chrono::system_clock::time_point cached_at;
    std::chrono::system_clock::time_point last_accessed;
    int access_count;
    
    bool is_expired() const;
};

// Client request structure
struct ClientRequest {
    std::string request_id;
    std::string client_ip;
    std::string operation; // "upload", "download", "list"
    std::string file_id;
    std::string filename;
    std::chrono::system_clock::time_point timestamp;
    
    json::value to_json() const;
    static ClientRequest from_json(const json::value& val);
};

// Utility functions
class Utils {
public:
    static double calculate_distance(double lat1, double lon1, double lat2, double lon2);
    static std::string generate_file_id();
    static std::string calculate_file_hash(const std::string& file_path);
    static std::string get_client_ip(const http_request& request);
    static json::value create_error_response(const std::string& error_msg);
    static json::value create_success_response(const json::value& data = json::value());
    static bool create_directory(const std::string& path);
    static std::vector<std::string> list_files_in_directory(const std::string& directory);
};

// Logging utility
class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };
    
    static void log(Level level, const std::string& component, const std::string& message);
    static void debug(const std::string& component, const std::string& message);
    static void info(const std::string& component, const std::string& message);
    static void warning(const std::string& component, const std::string& message);
    static void error(const std::string& component, const std::string& message);
};

// Configuration manager
class Config {
private:
    static std::unordered_map<std::string, std::string> config_map;
    
public:
    static bool load_from_file(const std::string& config_file);
    static std::string get(const std::string& key, const std::string& default_value = "");
    static int get_int(const std::string& key, int default_value = 0);
    static double get_double(const std::string& key, double default_value = 0.0);
    static bool get_bool(const std::string& key, bool default_value = false);
};

// Thread-safe LRU Cache implementation
template<typename K, typename V>
class LRUCache {
private:
    struct Node {
        K key;
        V value;
        std::shared_ptr<Node> prev;
        std::shared_ptr<Node> next;
        
        Node(const K& k, const V& v) : key(k), value(v) {}
    };
    
    size_t capacity_;
    std::unordered_map<K, std::shared_ptr<Node>> cache_map_;
    std::shared_ptr<Node> head_;
    std::shared_ptr<Node> tail_;
    mutable std::mutex mutex_;
    
    void move_to_front(std::shared_ptr<Node> node);
    void remove_node(std::shared_ptr<Node> node);
    void add_to_front(std::shared_ptr<Node> node);
    std::shared_ptr<Node> remove_tail();
    
public:
    explicit LRUCache(size_t capacity);
    
    bool get(const K& key, V& value) const;
    void put(const K& key, const V& value);
    bool remove(const K& key);
    size_t size() const;
    bool empty() const;
    void clear();
    std::vector<K> get_all_keys() const;
};

} // namespace cdn