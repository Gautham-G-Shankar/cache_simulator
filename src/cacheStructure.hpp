#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>

class CacheStructure {
private:
    int blockSize;
    int cacheSize;
    int associativity;
    int offsetBits;
    int indexBits;
    int tagBits;
    int cacheLines;
    int setvalidCount;
    int currentTime;


    struct cacheBlock {
        std::string tag;
        bool valid;
        bool dirty;
        int lastUsed;
    };


    std::vector<std::vector<cacheBlock>> cache;

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

    CacheStructure(int blockSize, int cacheSize, int associativity)
    : blockSize(blockSize) , cacheSize(cacheSize), associativity(associativity), currentTime(0) {
        cacheLines = cacheSize / (blockSize * associativity);
        offsetBits = std::log2(blockSize);
        indexBits = (associativity>1) ? std::log2(associativity) : std::log2(cacheLines);
        tagBits = 32 - (offsetBits + indexBits);
        

        cache.resize(cacheLines, std::vector<cacheBlock>(associativity, {"", false, false, 0}));
   }

   int hits, misses;

    int get_offset_bits() {return offsetBits;}
    int get_index_bits() {return indexBits;}
    int get_tag_bits() {return tagBits;}


    void access(const std::string& hexAddress, char operation) {
        std::string binaryAddress = hexToBinary(hexAddress);

        std::string tag = binaryAddress.substr(0, tagBits);
        std::string index = binaryAddress.substr(tagBits, indexBits);
        std::string offset = binaryAddress.substr(tagBits + indexBits, offsetBits);

        int indexValue = binaryToInt(index);
        bool hit = false;

        currentTime++;
        for (int i = 0; i < associativity; i++) {
            if (cache[indexValue][i].valid && cache[indexValue][i].tag == tag) {
                hit = true;
                cache[indexValue][i].lastUsed = currentTime;
                hits++;
                break;
            }
        }

        if (!hit) {
            misses++;

            if (associativity == 1) {

                cache[indexValue][0] = {tag, true, (operation == 'w'), currentTime};
            } 
            
            else {
                int lruIndex = 0;
                int minLastUsed = cache[indexValue][0].lastUsed;

                for (int i = 1; i < associativity; i++) {
                    if (!cache[indexValue][i].valid) {
                        lruIndex = i;
                        break;
                    }
                    
                    else if (cache[indexValue][i].lastUsed < minLastUsed) {
                        lruIndex = i;
                        minLastUsed = cache[indexValue][i].lastUsed;
                    }
                }

                cache[indexValue][lruIndex] = {tag, true, (operation == 'w'), currentTime};
            }
        }
    }

};