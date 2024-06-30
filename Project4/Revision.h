#ifndef REVISIONGENERATOR_H
#define REVISIONGENERATOR_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <functional>

//instance of the hashtable class
class HashTable {
public:
    HashTable() : capacity(131072), buckets(capacity) {
        if (buckets.size() == 0) {
            std::cerr << "HASH TABLE initialized with size: " << buckets.size() << std::endl;
            throw std::runtime_error("HashTable capacity must be non-zero");
        }
    }

    void insert(const std::string& key, size_t value) {
        size_t index = calculateIndex(key); // create valid index using hashFunction
        buckets[index].emplace_back(key, value); // put it in the bucket
    }

    bool contains(const std::string& key) {
        size_t index = calculateIndex(key); // finding the index where our key is at using hashFunction
        for (const auto& entry : buckets[index]) { // iterating through that bucket 
            if (entry.first == key) { return true; } // found an exact match
        }
        return false; // not found
    }

    size_t search(const std::string& key) {
        size_t index = calculateIndex(key);
        for (const auto& entry : buckets[index]) { // iterating through that bucket (list of pairs)
            if (entry.first == key) { return entry.second; } // found an exact match
        }
        throw std::runtime_error("Key not found in HashTable");
    }
    size_t calculateIndex(const std::string& key) { // finding the index where the key is at using hash
        if (buckets.size() == 0) {
            throw std::runtime_error("HashTable capacity must be non-zero");
        }
        return hashFunction(key) % buckets.size();
    }

    const std::list<std::pair<std::string, int>>& getBucketEntries(size_t index) const {
        return buckets.at(index);
    }
private:
    // vector, each element containing a list of pairs 
    size_t capacity;
    std::vector<std::list<std::pair<std::string, int>>> buckets;
    std::hash<std::string> hashFunction; // Hash function instance

};

char findDelimiter(const std::string& input, bool& delimiterFound) {
    bool existingChars[256] = { false }; //array of bools

    // Mark existing characters in the input string
    for (unsigned char c : input) {
        if (c >= 0 && c <= 255) {
            existingChars[c] = true;
        }
    }

    // Find the first character in the range 0 to 255 that does not exist in the input string
    for (unsigned char c = 0; c <= 255; ++c) {
        if (!existingChars[c]) {
            delimiterFound = true;
            return c; // Return the first character not found in the input string
        }
    }

    // If no delimiter is found, set the delimiterFound flag to false
    delimiterFound = false;
    return '~'; // Default delimiter if none found (not technically used here)
}


#endif 
