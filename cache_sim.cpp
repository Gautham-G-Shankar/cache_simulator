#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>

class DirectMappedCache {
private:
    int blockSize;
    int cacheSize;
    int numBlocks;
    int offsetBits;
    int indexBits;
    int tagBits;

    struct CacheBlock {
        std::string tag;
        bool valid;
    };

    std::vector<CacheBlock> cache;

    std::string hexToBinary(const std::string& hex) {
        std::string binary;
        for (char ch : hex) {
            int value = (ch >= '0' && ch <= '9') ? ch - '0' : ch - 'A' + 10;
            binary += std::bitset<4>(value).to_string();
        }
        return binary;
    }

    int binaryToInt(const std::string& binary) {
        int value = 0;
        for (char ch : binary) {
            value = (value << 1) + (ch - '0');
        }
        return value;
    }

public:
    DirectMappedCache(int blockSize, int cacheSize, int associativity)
        : blockSize(blockSize), cacheSize(cacheSize) {
        
        numBlocks = cacheSize / blockSize;
        offsetBits = std::log2(blockSize);
        indexBits = std::log2(numBlocks);
        tagBits = 32 - (indexBits + offsetBits);

        cache.resize(numBlocks, {"", false});
    }

    void access(const std::string& hexAddress, char operation) {
        std::string binaryAddress = hexToBinary(hexAddress);
        
        std::string tag = binaryAddress.substr(0, tagBits);
        std::string index = binaryAddress.substr(tagBits, indexBits);
        std::string offset = binaryAddress.substr(tagBits + indexBits, offsetBits);

        int indexValue = binaryToInt(index);

        
        
        if (cache[indexValue].valid && cache[indexValue].tag == tag) {
            std::cout << "Hit ";
        } else {
            cache[indexValue] = {tag, true};
            std::cout << "Miss ";
        }

        std::cout << cache[indexValue].tag << std::endl;
    }
};

