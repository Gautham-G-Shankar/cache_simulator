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
    CacheStructure cache = CacheStructure(blocksize, cachesize, associativity);


    // std::cout <<"Cache lines: " << cache.get_cachelines() << std::endl;

    

    if (_file_.is_open()) {
        while(std::getline(_file_,_line_)) {
            if (_line_.length()>=9) {
                cache.access(_line_.substr(2, 8), _line_.substr(0,1));

                // std::cout << "Read Count: " << _line_.substr(0,1) << " " << cache.get_read_count() << std::endl; 
                // std::cout << "Read Misses: "  << cache.get_read_misses() << std::endl;
            } 

            else {
                bitshortAddresses++;
                std::string lineVal = "00" + _line_;
                cache.access(lineVal.substr(2, 8), lineVal.substr(0,1));               
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

    std::cout << "Total Read Misses: " << cache.get_read_misses() << std::endl;
    std::cout << "Total Write Misses: " << cache.get_write_misses() << std::endl; 
    std::cout << "Total Read Hits: " << cache.get_read_hits() << std::endl;
    std::cout << "Total Write Hits: " << cache.get_write_hits() << std::endl; 
    std::cout << "Total Read: " << cache.get_read_count() << std::endl;
    std::cout << "Total Write: " << cache.get_write_count() << std::endl; 
    std::cout << "Current Time: " << cache.get_current_time() << std::endl;
    std::cout << "BitShort Addresses: " << bitshortAddresses << std::endl;
    // std::cout << "Total Lines: " << number_of_lines << std::endl; 

    cache.memory();
    // CacheStructure cacheL1 = CacheStructure();
    return 0;
}
