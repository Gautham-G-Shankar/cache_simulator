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
    int currentTime;
    int read_count;
    int read_hits, read_misses;
    int write_count;
    int write_hits, write_misses;
    std::string dirtyTag;
    bool dirtyHit;
    bool SwapDirty;
    std::string SwapAddress;
    int dirtyWriteCount;
    int offsetBits;
    int indexBits;
    int tagBits;
    int cacheLines;
    bool swapValid;



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


std::string binaryToHex(const std::string& binary) {
    
    int binaryLength = binary.length();
    
    int remainder = binaryLength % 4;
    
    std::string adjustedBinary = binary;

    if (remainder != 0) {
        std::string leadingBits = binary.substr(0, remainder);
        if (leadingBits.find_first_not_of('0') == std::string::npos) {
            adjustedBinary = binary.substr(remainder);
        }
        else {
            adjustedBinary = std::string(4 - remainder, '0') + binary;
        }
    }
    std::string hexString;
    for (size_t i = 0; i < adjustedBinary.size(); i += 4) {
        std::string fourBits = adjustedBinary.substr(i, 4);
        int decimalValue = std::bitset<4>(fourBits).to_ulong();  
        hexString += "0123456789abcdef"[decimalValue];  
    }

    return hexString;
}

public:



    CacheStructure(int blockSize, int cacheSize, int associativity)
    : blockSize(blockSize) , cacheSize(cacheSize), associativity(associativity), currentTime(0), read_count(0), read_hits(0), read_misses(0), write_count(0), write_hits(0), write_misses(0), dirtyTag(""), dirtyHit(false), SwapDirty(false), SwapAddress(""), dirtyWriteCount(0), swapValid(false) {
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
    bool get_swap_dirty()    {return SwapDirty;}
    std::string get_swap_address()    {return SwapAddress;}
    bool get_swap_valid()       {return swapValid;}


    std::pair<bool, int> checkHitOrMiss(const std::string& hexAddress) {
        std::string binaryAddress = hexToBinary(hexAddress);

        std::string tag = binaryAddress.substr(0, tagBits);
        std::string index = binaryAddress.substr(tagBits, indexBits);
        int indexValue = binaryToInt(index);

        swapValid = true;



        for (int i = 0; i < associativity; i++) {
            if (cache[indexValue][i].valid && cache[indexValue][i].tag == tag) {
                return {true, i};  
            }
        }

        for (int i = 0; i < associativity; i++) {
                swapValid = (swapValid && cache[indexValue][i].valid);
            }
        

        return {false, -1}; 
    }




    void accessCache(const std::string& hexAddress, const std::string& operation, bool hit, int blockIndex, bool VC) {
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

            for (int i = 0; i < associativity; i++) {
                if (!cache[indexValue][i].valid) {
                    lruIndex = i;
                    break;
                } else if (cache[indexValue][i].lastUsed < minLastUsed) {
                    lruIndex = i;
                    minLastUsed = cache[indexValue][i].lastUsed;
                }
            }

            if (cache[indexValue][lruIndex].dirty) {
                dirtyWriteCount++;
                dirtyHit = true;
                dirtyTag = cache[indexValue][lruIndex].tag + cache[indexValue][lruIndex].index + cache[indexValue][lruIndex].offset;
            }

            else {dirtyHit = false;}

            if (VC) {
                std::string SwapAddressBinary = cache[indexValue][lruIndex].tag + cache[indexValue][lruIndex].index + cache[indexValue][lruIndex].offset;
                SwapAddress = binaryToHex(SwapAddressBinary);
                SwapDirty = cache[indexValue][lruIndex].dirty;
            }



            cache[indexValue][lruIndex] = {tag, index, offset, true, (operation == "w"), currentTime};
        }
    }

    void dirtyWrite(std::string binaryAddress) {

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
        std::cout << "  set\t" << i << ":\t";

        std::vector<cacheBlock> sortedSet = cache[i]; 
        std::sort(sortedSet.begin(), sortedSet.end(), [](const cacheBlock& a, const cacheBlock& b) {
            return a.lastUsed > b.lastUsed;  
        });

        for (int j = 0; j < associativity; j++) {
            std::string printAddress = binaryToHex(sortedSet[j].tag);
            char dirtyBit = (sortedSet[j].dirty) ? 'D' : ' ';
            std::cout << printAddress  << " " << dirtyBit << "\t";
        }

        std::cout << std::endl;
    }
}
};


class FullyAssociativeCache {
private:
    int blockSize;
    int cacheLines;
    int currentTime;
    bool swapDirty;
    std::string swapAddress;
    int swapCount;
    std::string dirtyTag;
    bool dirtyHit;
    int dirtyWrites;
    int indexSwap;
    int offsetBits;
    int tagBits;

    
    struct cacheBlock {
        std::string tag;
        std::string offset;
        bool valid;
        bool dirty;
        int lastUsed;
    };

    std::vector<cacheBlock> cache;

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

std::string binaryToHex(const std::string& binary) {
    
    int binaryLength = binary.length();
    
    int remainder = binaryLength % 4;
    
    std::string adjustedBinary = binary;

    if (remainder != 0) {
        std::string leadingBits = binary.substr(0, remainder);
        if (leadingBits.find_first_not_of('0') == std::string::npos) {
            adjustedBinary = binary.substr(remainder);
        }
        else {
            adjustedBinary = std::string(4 - remainder, '0') + binary;
        }
    }
    std::string hexString;
    for (size_t i = 0; i < adjustedBinary.size(); i += 4) {
        std::string fourBits = adjustedBinary.substr(i, 4);
        int decimalValue = std::bitset<4>(fourBits).to_ulong();  
        hexString += "0123456789abcdef"[decimalValue];  
    }

    return hexString;
}

public:
    FullyAssociativeCache(int blockSize, int cacheLines) : blockSize(blockSize), cacheLines(cacheLines), currentTime(0), swapDirty(false),  swapAddress(""), swapCount(0), dirtyWrites(0), indexSwap(0), offsetBits(0), tagBits(0) {

        offsetBits = std::log2(blockSize);
        tagBits = 32 - offsetBits;
        cache.resize(cacheLines, {"", "",false, false, 0});
    }

    int get_dirty_writes() {return dirtyWrites;}
    int get_swap_count() {return swapCount;}
    std::string get_dirty_tag_address() {return dirtyTag;}
    bool get_dirty_hit()    {return dirtyHit;}



    std::pair<bool, int> checkHitOrMiss(const std::string& hexAddress) {

        std::string binaryAddress = hexToBinary(hexAddress);

        std::string _tag_ = binaryAddress.substr(0, tagBits);
        std::string tag = binaryToHex(_tag_);

        for (int i = 0; i < cacheLines; i++) {
            if (cache[i].valid && cache[i].tag == tag) {

                indexSwap = i;
                
                return {true, i};  
            }
        }
        return {false, -1}; 
    }

    void allocate(std::string _address_, bool _dirty_, bool hit) {
        currentTime++;
        std::string binaryAddress = hexToBinary(_address_);
        std::string _tag_ = binaryAddress.substr(0, tagBits);
        std::string _offset_ = binaryAddress.substr(tagBits);

        std::string tag = binaryToHex(_tag_);
        std::string offset = binaryToHex(_offset_);
        if (hit) {

            cache[indexSwap] = {tag, offset, true, _dirty_, currentTime};
            swapCount++;


        }
        else {

        int index = 0;
        int minIndex = cache[index].lastUsed;

        for (int i = 0; i<cacheLines; i++) {
            if (!cache[i].valid) {
                index = i;
                break;
            }

            else {
                if (cache[i].valid && cache[i].lastUsed < minIndex) {
                    index = i;
                    minIndex = cache[i].lastUsed;
                }
            }

        }

        if (cache[index].dirty) {
            dirtyHit = true;
            dirtyTag = hexToBinary(cache[index].tag + cache[index].offset);
            dirtyWrites++;
        }

        else {dirtyHit = false;}

        cache[index] = {tag, offset, true, _dirty_, currentTime};
        // std::cout << "Victim Cache Line: " << index << "\t" << cache[index].address << "LRU: " << cache[index].lastUsed << std::endl; 
    }



    }

    void memory() {


        std::vector<cacheBlock> sortedSet = cache; 
        std::sort(sortedSet.begin(), sortedSet.end(), [](const cacheBlock& a, const cacheBlock& b) {
            return a.lastUsed > b.lastUsed;  
        });

        for (int j = 0; j < cacheLines; j++) {

            std::cout << " set\t" << j << ":\t";

            char dirtyBit = (sortedSet[j].dirty) ? 'D' : ' ';
            std::cout << " " << cache[j].tag  << " " << dirtyBit << "\t";
            std::cout << std::endl;
        }


    
}

};