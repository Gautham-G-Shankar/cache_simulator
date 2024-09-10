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

        int totSwaps = 0;
        int blockSize = 16;
        int cacheSizeL1 = 1024;
        int associativity = 1;
        int numSetsL1 = cacheSizeL1 / (associativity * blockSize);
        int loop_count = -1;

        int cacheLinesVC = 16;

        CacheStructure cacheL1 = CacheStructure(blockSize, cacheSizeL1, associativity);
        FullyAssociativeCache cacheVC = FullyAssociativeCache(blockSize, cacheLinesVC);

        std::string _dirtyTag_;

        std::string address, operation;

    if (_file_.is_open()) {
        while (std::getline(_file_, _line_)) {

            loop_count++;

            operation = _line_.substr(0, 1);
            if (_line_.length() >= 9) {
                address =  _line_.substr(2);
            }

            else {
                address = "00" + _line_.substr(2);
            }

            auto [hitL1, blockIndexL1] = cacheL1.checkHitOrMiss(address);
            auto [hitVC, blockIndexVC] = cacheVC.checkHitOrMiss(address);

            if (!hitL1 && hitVC) {                
                cacheL1.accessCache(address, operation, false, blockIndexL1, true);
                std::string addressVC = cacheL1.get_swap_address();
                bool dirtyVC = cacheL1.get_swap_dirty();
                cacheVC.allocate(addressVC, dirtyVC, true);

                totSwaps++;
            }

            else if (!hitL1 && !hitVC) {
                cacheL1.accessCache(address, operation, false, blockIndexL1, true);
                std::string addressVC = cacheL1.get_swap_address();
                bool dirtyVC = cacheL1.get_swap_dirty();
                cacheVC.allocate(addressVC, dirtyVC, false);
            }

            else {
                cacheL1.accessCache(address, operation, true, blockIndexL1, false);
            }

    }

         _file_.close();
}
    std::cout << "Total Read Misses L1: \t" << cacheL1.get_read_misses() << std::endl;
    std::cout << "Total Write Misses L1: \t" << cacheL1.get_write_misses() << std::endl; 
    std::cout << "Total Read Hits L1: \t" << cacheL1.get_read_hits() << std::endl;
    std::cout << "Total Write Hits L1: \t" << cacheL1.get_write_hits() << std::endl; 
    std::cout << "Total Read L1: \t\t" << cacheL1.get_read_count() << std::endl;
    std::cout << "Total Write L1: \t" << cacheL1.get_write_count() << std::endl; 
    std::cout << "Current Time L1: \t" << cacheL1.get_current_time() << std::endl;
    std::cout << "Swap Count: " << totSwaps << std::endl;
    
    cacheL1.memory();

    cacheVC.memory();
return 0;

}