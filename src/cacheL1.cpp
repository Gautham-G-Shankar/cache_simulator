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


    

    if (_file_.is_open()) {
        while(std::getline(_file_,_line_)) {

            if (_line_.length()>8) {

                auto [hit, blockIndex] = cacheL1.checkHitOrMiss(_line_.substr(2, 8));
                cacheL1.accessCache(_line_.substr(2, 8), _line_.substr(0,1), hit, blockIndex);

            } 

            else {
                bitshortAddresses++;
                std::string lineVal = _line_.substr(0, 1) + " " + "00" + _line_.substr(2, 8);
                std::cout << "The value is " << lineVal << std::endl;
                auto [hit, blockIndex] = cacheL1.checkHitOrMiss(lineVal.substr(2, 8));
                cacheL1.accessCache(lineVal.substr(2, 8), lineVal.substr(0,1), hit, blockIndex);            
            }


        } 
     _file_.close();
    }


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