#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>
#include "../Assignment_files/cacheStructure.hpp"
#include <fstream>


int main(){
    std::string _line_;
    std::ifstream _file_("gcc_trace.txt");

    int blocksize = 16;
    int cachesize = 1024;
    int associativity = 2;
    int numSets = cachesize / (associativity * blocksize);
    int bitshortAddresses = 0;
    CacheStructure cacheL1 = CacheStructure(blocksize, cachesize, associativity);


    // std::cout <<"Cache lines: " << cache.get_cachelines() << std::endl;

    

    if (_file_.is_open()) {
        while(std::getline(_file_,_line_)) {

            if (_line_.length()>8) {

                auto [hit, blockIndex] = cacheL1.checkHitOrMiss(_line_.substr(2, 8));
                cacheL1.accessCache(_line_.substr(2, 8), _line_.substr(0,1), hit, blockIndex);

                // std::cout << "Read Count: " << _line_.substr(0,1) << " " << cache.get_read_count() << std::endl; 
                // std::cout << "Read Misses: "  << cache.get_read_misses() << std::endl;
            } 

            else {
                bitshortAddresses++;
                std::string lineVal = "00" + _line_;
                auto [hit, blockIndex] = cacheL1.checkHitOrMiss(lineVal.substr(2, 8));
                cacheL1.accessCache(lineVal.substr(2, 8), lineVal.substr(0,1), hit, blockIndex);            
            }


        } 
     _file_.close();
    }

    // for (int i=0; i<numSets; i++) {

    //     std::cout << "Set   " << i << ":    ";

    //     for (int j=0; j<associativity; j++) {
    //         std::cout << cache[i][j].tag << " " << cache[i][j].dirty;   
    //     }

    //     std::cout << "" << std::endl;
    // }

    // cache.memory();

    std::cout << "Total Read Misses: " << cacheL1.get_read_misses() << std::endl;
    std::cout << "Total Write Misses: " << cacheL1.get_write_misses() << std::endl; 
    std::cout << "Total Read Hits: " << cacheL1.get_read_hits() << std::endl;
    std::cout << "Total Write Hits: " << cacheL1.get_write_hits() << std::endl; 
    std::cout << "Total Read: " << cacheL1.get_read_count() << std::endl;
    std::cout << "Total Write: " << cacheL1.get_write_count() << std::endl; 
    std::cout << "Current Time: " << cacheL1.get_current_time() << std::endl;
    std::cout << "BitShort Addresses: " << bitshortAddresses << std::endl;
    // std::cout << "Total Lines: " << number_of_lines << std::endl; 

    cacheL1.memory();
    // CacheStructure cacheL1 = CacheStructure();
    return 0;
}