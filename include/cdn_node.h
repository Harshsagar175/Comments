#pragma once

#include "common.h"
#include <unordered_set>

namespace cdn {

class CDNNode {
private:
    std::string node_id_;
    std::string ip_address_;
    int port_;
    double latitude_;
    double longitude_;
    
    // HTTP server
    std::unique_ptr<http_listener> listener_;
    
    // Cache management
    LRUCache<std::string, CacheEntry> file_cache_;
    std::string cache_directory_;
    size_t cache_capacity_;
    std::atomic<size_t> cache_usage_;
    mutable std::mutex cache_mutex_;
    
    // Communication with meta server
    std::string meta_server_url_;
    std::unique_ptr<http_client> meta_client_;
    
    // Communication with FSS
    std::string fss_url_;
    std::unique_ptr<http_client> fss_client_;
    
    // Heartbeat and status
    std::atomic<bool> is_running_;
    std::thread heartbeat_thread_;
    std::chrono::seconds heartbeat_interval_;
    
    // Request handlers
    void handle_get(http_request request);
    void handle_post(http_request request);
    void handle_put(http_request request);
    void handle_delete(http_request request);
    
    // File operations
    pplx::task<void> download_file(const std::string& file_id);
    pplx::task<bool> upload_file_to_fss(const std::string& file_path, const std::string& file_id);
    bool cache_file(const std::string& file_id, const std::string& file_path);
    bool remove_from_cache(const std::string& file_id);
    std::string get_cache_file_path(const std::string& file_id) const;
    
    // Cache management
    void cleanup_expired_cache();
    void evict_lru_files(size_t bytes_needed);
    
    // Communication
    pplx::task<void> send_heartbeat();
    pplx::task<void> register_with_meta_server();
    pplx::task<void> notify_meta_server_file_cached(const std::string& file_id);
    pplx::task<void> notify_meta_server_file_removed(const std::string& file_id);
    
    // Prefetching
    pplx::task<void> prefetch_directory_files(const std::string& file_id);
    
public:
    CDNNode(const std::string& node_id, const std::string& ip_address, int port,
            double latitude, double longitude, const std::string& cache_dir,
            size_t cache_capacity, const std::string& meta_server_url,
            const std::string& fss_url);
    
    ~CDNNode();
    
    // Lifecycle
    pplx::task<void> start();
    pplx::task<void> stop();
    bool is_running() const { return is_running_.load(); }
    
    // Getters
    const std::string& get_node_id() const { return node_id_; }
    const std::string& get_ip_address() const { return ip_address_; }
    int get_port() const { return port_; }
    double get_latitude() const { return latitude_; }
    double get_longitude() const { return longitude_; }
    size_t get_cache_usage() const { return cache_usage_.load(); }
    size_t get_cache_capacity() const { return cache_capacity_; }
    
    // Cache operations
    bool has_file_cached(const std::string& file_id) const;
    std::vector<std::string> get_cached_files() const;
    
    // Statistics
    json::value get_statistics() const;
};

} // namespace cdn