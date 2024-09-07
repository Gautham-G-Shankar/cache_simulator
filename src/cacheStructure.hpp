#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>
#include <algorithm>

class CacheStructure {
private:
    int blockSize;
    int cacheSize;
    int associativity;
    int offsetBits;
    int indexBits;
    int tagBits;
    int cacheLines;
    int currentTime;
    int read_hits, read_misses, write_hits, write_misses;
    int read_count, write_count;
    std::string dirtyTag;
    bool dirtyHit;
    int dirtyWriteCount;

    struct cacheBlock {
        std::string tag;
        std::string index;
        std::string offset;
        bool valid;
        bool dirty;
        int lastUsed;
    };

    std::vector<std::vector<cacheBlock>> cache;

    std::string hexToBinary(const std::string& hex) {
        std::string binary;
        for (char ch : hex) {
            int value = (ch >= '0' && ch <= '9') ? ch - '0' : ch - 'a' + 10;
            binary += std::bitset<4>(value).to_string();
        }
        return binary;
    }

    int binaryToInt(const std::string& binary) {
        int value = 0;
        for (char ch : binary) {
            value = (value * 2) + (ch - '0');
        }
        return value;
    }

public:

    CacheStructure(int blockSize, int cacheSize, int associativity)
    : blockSize(blockSize) , cacheSize(cacheSize), associativity(associativity), currentTime(0), read_count(0), read_hits(0), read_misses(0), write_count(0), write_hits(0), write_misses(0), dirtyTag(""), dirtyHit(false) {
        cacheLines = cacheSize / (blockSize * associativity);
        offsetBits = std::log2(blockSize);
        indexBits = std::log2(cacheLines);
        tagBits = 32 - (offsetBits + indexBits);

        cache.resize(cacheLines, std::vector<cacheBlock>(associativity, {"", "", "", false, false, 0}));
   }

    int get_cachelines()    {return cacheLines;}
    int get_offset_bits()   {return offsetBits;}
    int get_index_bits()    {return indexBits;}
    int get_tag_bits()      {return tagBits;}
    int get_read_misses()   {return read_misses;}
    int get_write_misses()  {return write_misses;}
    int get_read_count()    {return read_count;}
    int get_write_count()   {return write_count;}
    int get_read_hits()     {return read_hits;}
    int get_write_hits()    {return write_hits;}
    int get_current_time()  {return currentTime;}
    int get_dirty_writes()  {return dirtyWriteCount;}
    std::string get_dirty_tag_address() {return dirtyTag;}
    bool get_dirty_hit() {return dirtyHit;}

    std::pair<bool, int> checkHitOrMiss(const std::string& hexAddress) {
        std::string binaryAddress = hexToBinary(hexAddress);

        std::string tag = binaryAddress.substr(0, tagBits);
        std::string index = binaryAddress.substr(tagBits, indexBits);
        int indexValue = binaryToInt(index);

        for (int i = 0; i < associativity; i++) {
            if (cache[indexValue][i].valid && cache[indexValue][i].tag == tag) {
                return {true, i};  
            }
        }
        return {false, -1}; 
    }


    void accessCache(const std::string& hexAddress, const std::string& operation, bool hit, int blockIndex) {
        currentTime++;

        std::string binaryAddress = hexToBinary(hexAddress);
        std::string tag = binaryAddress.substr(0, tagBits);
        std::string index = binaryAddress.substr(tagBits, indexBits);
        std::string offset = binaryAddress.substr(tagBits + indexBits, offsetBits);


        int indexValue = binaryToInt(index);

        if (operation == "w") {
            write_count++;
        } else {
            read_count++;
        }

        if (hit) {
            cache[indexValue][blockIndex].lastUsed = currentTime;

            if (operation == "w") {
                write_hits++;

            cache[indexValue][blockIndex].dirty = true; 
            
            } 
            
            else {
                read_hits++;
            }
        } 
            
        else {
            
            if (operation == "w") {
                write_misses++;
            } else {
                read_misses++;
            }

            int lruIndex = 0;
            int minLastUsed = cache[indexValue][0].lastUsed;

            for (int i = 1; i < associativity; i++) {
                if (!cache[indexValue][i].valid) {
                    lruIndex = i;
                    break;
                } else if (cache[indexValue][i].lastUsed < minLastUsed) {
                    lruIndex = i;
                    minLastUsed = cache[indexValue][i].lastUsed;
                }
            }

            if (cache[indexValue][lruIndex].dirty) {
                dirtyHit = true;
                dirtyTag = cache[indexValue][lruIndex].tag + cache[indexValue][lruIndex].index + cache[indexValue][lruIndex].offset;
            }

            else {dirtyHit = false;}


            cache[indexValue][lruIndex] = {tag, index, offset, true, (operation == "w"), currentTime};
        }
    }

    void dirtyWrite(std::string binaryAddress) {

            dirtyWriteCount++;

            currentTime++;

            write_count++;

            std::string _tag_ = binaryAddress.substr(0, tagBits);
            std::string _index_ = binaryAddress.substr(tagBits, indexBits);
            std::string _offset_ = binaryAddress.substr(tagBits + indexBits, offsetBits);

            bool HIT = false;

            int _index_Val = binaryToInt(_index_);

            int _lruIndex_ = 0;
            int minLastUsed = cache[_index_Val][0].lastUsed;

            for (int i = 0; i < associativity; i++) {
                if ((cache[_index_Val][i].valid && cache[_index_Val][i].tag == _tag_) ) {    
                    _lruIndex_ = i;
                    write_hits++;
                    HIT = true;
                    break;
                }
            }

        if (!HIT) {

            write_misses++;

            for (int i = 1; i < associativity; i++) {
                if (!cache[_index_Val][i].valid) {
                    _lruIndex_ = i;
                    break;
                } else if (cache[_index_Val][i].lastUsed < minLastUsed) {
                    _lruIndex_ = i;
                    minLastUsed = cache[_index_Val][i].lastUsed;
                }
            }
        }

            cache[_index_Val][_lruIndex_] = {_tag_, _index_, _offset_, true, true, currentTime};


    }


    void memory() {
    for (int i = 0; i < cacheLines; i++) {
        std::cout << "Set\t" << i;

        std::vector<cacheBlock> sortedSet = cache[i]; 
        std::sort(sortedSet.begin(), sortedSet.end(), [](const cacheBlock& a, const cacheBlock& b) {
            return a.lastUsed > b.lastUsed;  
        });

        for (int j = 0; j < associativity; j++) {
            char dirtyBit = (sortedSet[j].dirty) ? 'D' : ' ';
            std::cout << ":\t" << sortedSet[j].tag  << " " << dirtyBit << "\t";
        }

        std::cout << std::endl;
    }
}
};