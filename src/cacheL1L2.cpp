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

    int blocksizeL1 = 16;
    int cachesizeL1 = 1024;
    int associativityL1 = 1;
    int numSetsL1 = cachesizeL1 / (associativityL1 * blocksizeL1);

    int blocksizeL2 = 16;
    int cachesizeL2 = 8192;
    int associativityL2 = 4;
    int numSetsL2 = cachesizeL2 / (associativityL2 * blocksizeL2);

    CacheStructure cacheL1 = CacheStructure(blocksizeL1, cachesizeL1, associativityL1);
    CacheStructure cacheL2 = CacheStructure(blocksizeL2, cachesizeL2, associativityL2);

    std::string _dirtyTag_;

    std::string address, operation;

    if (_file_.is_open()) {
        while (std::getline(_file_, _line_)) {

            operation = _line_.substr(0, 1);

            if (_line_.length() < 9) {
                address = "00" + _line_.substr(2);
            }

            else {
                address = _line_.substr(2);
            }
            
            auto [hitL1, blockIndexL1] = cacheL1.checkHitOrMiss(address);
            auto [hitL2, blockIndexL2] = cacheL2.checkHitOrMiss(address);

            if (operation == "r") {
                
                if (hitL1) {
                    cacheL1.accessCache(address, "r", true, blockIndexL1, false);
                }

                else if (!hitL1 && hitL2) {
                    cacheL1.accessCache(address, "r", false, blockIndexL1, false);
                    cacheL2.accessCache(address, "r", true, blockIndexL2, false);

                    _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);} 
                }

                else {
                    cacheL1.accessCache(address, "r", false, blockIndexL1, false);
                    cacheL2.accessCache(address, "r", false, blockIndexL2, false);

                    _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}



                }

            }

            else {
                if (hitL1) {
                    cacheL1.accessCache(address, "w", true, blockIndexL1, false);
                    // _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    // if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}

                }

                // else if (!hitL1 && hitL2) {
                //     cacheL2.accessCache(address, "w", true, blockIndexL2);
                // }

                else {
                    cacheL1.accessCache(address, "w", false, blockIndexL1, false);
                    _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}

                    cacheL2.accessCache(address, "r", hitL2, blockIndexL2, false);

                    // std::cout << "Hit L2: " << hitL2 << std::endl;


                }
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

    std::cout << "Total Read Misses L2: \t" << cacheL2.get_read_misses() << std::endl;
    std::cout << "Total Write Misses L2: \t" << cacheL2.get_write_misses() << std::endl; 
    std::cout << "Total Read Hits L2: \t" << cacheL2.get_read_hits() << std::endl;
    std::cout << "Total Write Hits L2: \t" << cacheL2.get_write_hits() << std::endl; 
    std::cout << "Total Read L2: \t\t" << cacheL2.get_read_count() << std::endl;
    std::cout << "Total Write L2: \t" << cacheL2.get_write_count() << std::endl; 
    std::cout << "Current Time L2: \t" << cacheL2.get_current_time() << std::endl;

    std::cout << "Dirty Writes: \t" << cacheL2.get_dirty_writes() << std::endl;

    cacheL2.memory();

    return 0;
}