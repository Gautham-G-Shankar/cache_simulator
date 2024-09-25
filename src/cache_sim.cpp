#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <bitset>
#include "../Assignment_files/cacheStructure.hpp"
#include "../Assignment_files/parse.h"
#include <fstream>

int main(int argc, char *argv[]) {



    int cacheSizeL1     = std::stoi(argv[1]);
    int associativityL1 = std::stoi(argv[2]);
    int blockSize       = std::stoi(argv[3]);
    int cacheLinesVC    = std::stoi(argv[4]);
    int cacheSizeL2     = std::stoi(argv[5]);
    int associativityL2 = std::stoi(argv[6]);

    std::string file_name = argv[7];

    std::string _line_;
    std::ifstream _file_(file_name);
    
    bool VC = (cacheLinesVC != 0);
    bool L2 = (cacheSizeL2 != 0);
    float mainMemoryPenalty = (20.0f + ( 16.0 / 16.0));


    if (!VC && !L2) {

    CacheStructure cacheL1 = CacheStructure(blockSize, cacheSizeL1, associativityL1);

    if (_file_.is_open()) {
        while(std::getline(_file_,_line_)) {

            if (_line_.length()>8) {

                auto [hit, blockIndex] = cacheL1.checkHitOrMiss(_line_.substr(2, 8));
                cacheL1.accessCache(_line_.substr(2, 8), _line_.substr(0,1), hit, blockIndex, false);

            } 

            else {
                std::string lineVal = _line_.substr(0, 1) + " " + "00" + _line_.substr(2, 8);
                auto [hit, blockIndex] = cacheL1.checkHitOrMiss(lineVal.substr(2, 8));
                cacheL1.accessCache(lineVal.substr(2, 8), lineVal.substr(0,1), hit, blockIndex, false);            
            }


        } 
     _file_.close();
    }

    float combinedMissrateL1 = (static_cast<float> (cacheL1.get_read_misses() + cacheL1.get_write_misses()) / (cacheL1.get_read_count() + cacheL1.get_write_count()));
    int total_memory_traffic = cacheL1.get_read_misses() + cacheL1.get_write_misses() + cacheL1.get_dirty_writes();

    float accessTime, energy, area;
    int result = get_cacti_results(cacheSizeL1, blockSize, associativityL1, &accessTime, &energy, &area);
    
    if (result==0) {

    
    float AAT = accessTime + static_cast<float>(combinedMissrateL1) * mainMemoryPenalty;
    float energyDelayProduct = (energy* static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_read_count() + cacheL1.get_write_count() + cacheL1.get_write_misses()) + 0.05* static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_write_misses() + cacheL1.get_dirty_writes()))*AAT * 100000.0f;

    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << "  L1_SIZE:\t\t" << cacheSizeL1 << std::endl;
    std::cout << "  L1_ASSOC:\t\t" << associativityL1 << std::endl;
    std::cout << "  L1_BLOCKSIZE:\t\t" << blockSize << std::endl;
    std::cout << "  VC_NUM_BLOCKS:\t" << cacheLinesVC << std::endl;
    std::cout << "  L2_SIZE:\t\t" << cacheSizeL2 << std::endl;
    std::cout << "  L2_ASSOC:\t\t" << associativityL2 << std::endl;
    std::cout << "  trace_file:\t\t" << file_name << std::endl;
    std::cout << std::endl;

    std::cout << "===== L1 contents =====" << std::endl;
    cacheL1.memory();
    std::cout << std::endl;

    std::cout << "===== Simulation results (raw) =====" << std::endl;
    std::cout << "  a. number of L1 reads:\t\t\t\t" << cacheL1.get_read_count() << std::endl;
    std::cout << "  b. number of L1 read misses:\t\t\t\t" << cacheL1.get_read_misses() << std::endl;
    std::cout << "  c. number of L1 writes:\t\t\t\t" << cacheL1.get_write_count() << std::endl;
    std::cout << "  d. number of L1 write misses:\t\t\t\t" << cacheL1.get_write_misses() << std::endl;
    std::cout << "  e. number of swap requests:\t\t\t\t0" << std::endl;
    std::cout << "  f. swap request rate:\t\t\t\t\t0.0000" << std::endl;
    std::cout << "  g. number of swaps:\t\t\t\t\t0" << std::endl;
    std::cout << "  h. combined L1+VC miss rate:\t\t\t\t"  << combinedMissrateL1 << std::endl;
    std::cout << "  i. number writebacks from L1/VC:\t\t\t" << cacheL1.get_dirty_writes() << std::endl;
    std::cout << "  j. number of L2 reads:\t\t\t\t0" << std::endl;
    std::cout << "  k. number of L2 read misses:\t\t\t\t0" << std::endl;
    std::cout << "  l. number of L2 writes:\t\t\t\t0" << std::endl;
    std::cout << "  m. number of L2 write misses:\t\t\t\t0" << std::endl;
    std::cout << "  n. L2 miss rate:\t\t\t\t\t0.0000" << std::endl;
    std::cout << "  o. number of writebacks from L2:\t\t\t0" << std::endl;
    std::cout << "  p. total memory traffic:\t\t\t\t" << total_memory_traffic << std::endl;

    std::cout << std::endl;

    std::cout << "===== Simulation results (performance) =====" << std::endl;
    std::cout << "  1. average access time:\t\t\t" << AAT << std::endl;
    std::cout << "  2. energy-delay product:\t\t\t" << energyDelayProduct << std::endl;
    std::cout << "  3. total area:\t\t\t\t" << area << std::endl;


    }
}

else if (!VC && L2) {
    CacheStructure cacheL1 = CacheStructure(blockSize, cacheSizeL1, associativityL1);
    CacheStructure cacheL2 = CacheStructure(blockSize, cacheSizeL2, associativityL2);

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
                }


                else {
                    cacheL1.accessCache(address, "w", false, blockIndexL1, false);
                    _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}

                    cacheL2.accessCache(address, "r", hitL2, blockIndexL2, false);


                }
            }


        }

     _file_.close();

    }

    float combinedMissrateL1 = (static_cast<float> (cacheL1.get_read_misses() + cacheL1.get_write_misses()) / (cacheL1.get_read_count() + cacheL1.get_write_count()));
    float combinedMissrateL2 = (static_cast<float> (cacheL2.get_read_misses() + cacheL2.get_write_misses()) / (cacheL2.get_read_count() + cacheL2.get_write_count()));
    int total_memory_traffic = cacheL2.get_read_misses() + cacheL2.get_write_misses() + cacheL2.get_dirty_writes();
    float missRateL2 =  static_cast<float> (cacheL2.get_read_misses() / cacheL2.get_read_count());
    float accessTimeL1, energyL1, areaL1;
    float accessTimeL2, energyL2, areaL2;
    int resultL1 = get_cacti_results(cacheSizeL1, blockSize, associativityL1, &accessTimeL1, &energyL1, &areaL1);
    int resultL2 = get_cacti_results(cacheSizeL2, blockSize, associativityL2, &accessTimeL2, &energyL2, &areaL2);
    if (!resultL1 && !resultL2) {

    float AATL2 = accessTimeL2 + static_cast<float>(combinedMissrateL2) * mainMemoryPenalty;
    float AAT = accessTimeL1 + static_cast<float>(combinedMissrateL1) * AATL2;
    float energyDelayProduct = (energyL1* static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_read_count() + cacheL1.get_write_count() + cacheL1.get_write_misses()) + 0.05* static_cast<float>(cacheL2.get_read_misses() + cacheL2.get_write_misses() + cacheL2.get_dirty_writes()) + energyL2 * static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_read_count() + cacheL1.get_write_count() + cacheL1.get_write_misses()))*AAT * 100000.0f;
    float area = areaL1 + areaL2;
    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << "  L1_SIZE:\t\t" << cacheSizeL1 << std::endl;
    std::cout << "  L1_ASSOC:\t\t" << associativityL1 << std::endl;
    std::cout << "  L1_BLOCKSIZE:\t\t" << blockSize << std::endl;
    std::cout << "  VC_NUM_BLOCKS:\t" << cacheLinesVC << std::endl;
    std::cout << "  L2_SIZE:\t\t" << cacheSizeL2 << std::endl;
    std::cout << "  L2_ASSOC:\t\t" << associativityL2 << std::endl;
    std::cout << "  trace_file:\t\t" << file_name << std::endl;
    std::cout << std::endl;

    std::cout << "===== L1 contents =====" << std::endl;
    cacheL1.memory();
    std::cout << std::endl;

    std::cout << "===== L2 contents =====" << std::endl;
    cacheL2.memory();
    std::cout << std::endl;


    std::cout << "===== Simulation results (raw) =====" << std::endl;
    std::cout << "  a. number of L1 reads:\t\t\t\t" << cacheL1.get_read_count() << std::endl;
    std::cout << "  b. number of L1 read misses:\t\t\t\t" << cacheL1.get_read_misses() << std::endl;
    std::cout << "  c. number of L1 writes:\t\t\t\t" << cacheL1.get_write_count() << std::endl;
    std::cout << "  d. number of L1 write misses:\t\t\t\t" << cacheL1.get_write_misses() << std::endl;
    std::cout << "  e. number of swap requests:\t\t\t\t0" << std::endl;
    std::cout << "  f. swap request rate:\t\t\t\t\t0.0000" << std::endl;
    std::cout << "  g. number of swaps:\t\t\t\t\t0" << std::endl;
    std::cout << "  h. combined L1+VC miss rate:\t\t\t\t"  << combinedMissrateL1 << std::endl;
    std::cout << "  i. number writebacks from L1/VC:\t\t\t" << cacheL1.get_dirty_writes() << std::endl;
    std::cout << "  j. number of L2 reads:\t\t\t\t" << cacheL2.get_read_count() << std::endl;
    std::cout << "  k. number of L2 read misses:\t\t\t\t" << cacheL2.get_read_misses() << std::endl;
    std::cout << "  l. number of L2 writes:\t\t\t\t" << cacheL2.get_write_count() << std::endl;
    std::cout << "  m. number of L2 write misses:\t\t\t\t" << cacheL2.get_write_misses() << std::endl;
    std::cout << "  n. L2 miss rate:\t\t\t\t\t" << missRateL2 << std::endl;
    std::cout << "  o. number of writebacks from L2:\t\t\t" << cacheL2.get_dirty_writes() << std::endl;
    std::cout << "  p. total memory traffic:\t\t\t\t" << total_memory_traffic << std::endl;

    std::cout << std::endl;

    std::cout << "===== Simulation results (performance) =====" << std::endl;
    std::cout << "  1. average access time:\t\t\t" << AAT << std::endl;
    std::cout << "  2. energy-delay product:\t\t\t" << energyDelayProduct << std::endl;
    std::cout << "  3. total area:\t\t\t\t" << area << std::endl;


    }

}

else if (VC && !L2) {

    CacheStructure cacheL1 = CacheStructure(blockSize, cacheSizeL1, associativityL1);
    FullyAssociativeCache cacheVC = FullyAssociativeCache(blockSize, cacheLinesVC);

    std::string _dirtyTag_;
    std::string address, operation;
    int totSwaps = 0;
    int VCMisses = 0;
    int totSwapReq = 0;
    int totAccess = 0;
    int Wb = 0;

    if (_file_.is_open()) {
        while (std::getline(_file_, _line_)) {

            operation = _line_.substr(0, 1);
            if (_line_.length() >= 9) {
                address =  _line_.substr(2);
            }

            else {
                address = "00" + _line_.substr(2);
            }

            auto [hitL1, blockIndexL1] = cacheL1.checkHitOrMiss(address);
            auto [hitVC, blockIndexVC] = cacheVC.checkHitOrMiss(address);

            if (hitL1) {
                cacheL1.accessCache(address, operation, true, blockIndexL1, false);
            }


            else if (!hitL1 && hitVC) {                
                cacheL1.accessCache(address, operation, false, blockIndexL1, true);
                std::string addressVC = cacheL1.get_swap_address();
                bool dirtyVC = cacheL1.get_swap_dirty();


                // if (cacheL1.get_swap_valid()) {
                    cacheVC.allocate(addressVC, dirtyVC, true);
                    totSwaps++;
                    totSwapReq++;
                    // }

                // cacheVC.memory();
                totAccess++;
            }

            else if (!hitL1 && !hitVC) {
                cacheL1.accessCache(address, operation, false, blockIndexL1, true);
                std::string addressVC = cacheL1.get_swap_address();
                bool dirtyVC = cacheL1.get_swap_dirty();

                // if (addressVC != "") {cacheVC.allocate(addressVC, dirtyVC, false);}

                // cacheVC.memory();
                if (cacheL1.get_swap_valid()) {
                    cacheVC.allocate(addressVC, dirtyVC, false);
                    VCMisses++;
                    totSwapReq++;
                    }
                
                if (cacheVC.get_dirty_hit()) {Wb++;}


                totAccess++;
            } 

    }

         _file_.close();
}


    float accessTimeL1, energyL1, areaL1;
    float accessTimeVC, energyVC, areaVC;
    int VCSize = blockSize * cacheLinesVC;
    int AssociativityVC = (VCSize / blockSize);
    int resultL1 = get_cacti_results(cacheSizeL1, blockSize, associativityL1, &accessTimeL1, &energyL1, &areaL1);
    int resultVC = get_cacti_results(VCSize, blockSize, AssociativityVC, &accessTimeVC, &energyVC, &areaVC);


    if (!resultL1 && !resultVC) {


    // float combinedMissrateL1 = (static_cast<float> (cacheL1.get_read_misses() + cacheL1.get_write_misses() - totSwaps) / (cacheL1.get_read_count() + cacheL1.get_write_count()));
    // float VCMissRate = static_cast<float> (VCMisses / totAccess);
    // float AATVC = accessTimeVC * (1.0000000 - VCMissRate) + static_cast<float>(VCMissRate) * mainMemoryPenalty;
    // float AAT = accessTimeL1 + static_cast<float>(combinedMissrateL1) * AATVC;
    // float energyDelayProduct = (energyL1* static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_read_count() + cacheL1.get_write_count() + cacheL1.get_write_misses()) + 0.05* static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_write_misses() + cacheVC.get_dirty_writes() - totSwaps) + energyVC * 2 * totSwapReq ) * AAT * 100000.0f;
    float area = areaL1 + areaVC;
    float swapRequestRate = static_cast<float>(totSwapReq / static_cast<float>(cacheL1.get_read_count() + cacheL1.get_write_count()));
    int total_memory_traffic = cacheL1.get_read_misses() + cacheL1.get_write_misses() + cacheVC.get_dirty_writes() - totSwaps;


    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << "  L1_SIZE:\t\t" << cacheSizeL1 << std::endl;
    std::cout << "  L1_ASSOC:\t\t" << associativityL1 << std::endl;
    std::cout << "  L1_BLOCKSIZE:\t\t" << blockSize << std::endl;
    std::cout << "  VC_NUM_BLOCKS:\t" << cacheLinesVC << std::endl;
    std::cout << "  L2_SIZE:\t\t" << cacheSizeL2 << std::endl;
    std::cout << "  L2_ASSOC:\t\t" << associativityL2 << std::endl;
    std::cout << "  trace_file:\t\t" << file_name << std::endl;
    std::cout << std::endl;

    std::cout << "===== L1 contents =====" << std::endl;
    cacheL1.memory();
    std::cout << std::endl;

    std::cout << "===== VC contents =====" << std::endl;
    cacheVC.memory();
    std::cout << std::endl;


    std::cout << "===== Simulation results (raw) =====" << std::endl;
    std::cout << "  a. number of L1 reads:\t\t\t\t" << cacheL1.get_read_count() << std::endl;
    std::cout << "  b. number of L1 read misses:\t\t\t\t" << cacheL1.get_read_misses() << std::endl;
    std::cout << "  c. number of L1 writes:\t\t\t\t" << cacheL1.get_write_count() << std::endl;
    std::cout << "  d. number of L1 write misses:\t\t\t\t" << cacheL1.get_write_misses() << std::endl;
    std::cout << "  e. number of swap requests:\t\t\t\t" << totSwapReq << std::endl;
    std::cout << "  f. swap request rate:\t\t\t\t\t" << swapRequestRate << std::endl;
    std::cout << "  g. number of swaps:\t\t\t\t\t" << cacheVC.get_swap_count() << " " << totSwaps << std::endl;
    // std::cout << "  h. combined L1+VC miss rate:\t\t\t\t"  << combinedMissrateL1 << std::endl;
    std::cout << "  i. number writebacks from L1/VC:\t\t\t" << Wb << std::endl;
    std::cout << "  j. number of L2 reads:\t\t\t\t0" << std::endl;
    std::cout << "  k. number of L2 read misses:\t\t\t\t0" << std::endl;
    std::cout << "  l. number of L2 writes:\t\t\t\t0" << std::endl;
    std::cout << "  m. number of L2 write misses:\t\t\t\t0" << std::endl;
    std::cout << "  n. L2 miss rate:\t\t\t\t\t0.0000" << std::endl;
    std::cout << "  o. number of writebacks from L2:\t\t\t0" << std::endl;
    std::cout << "  p. total memory traffic:\t\t\t\t" << total_memory_traffic << std::endl;

    std::cout << std::endl;

    std::cout << "===== Simulation results (performance) =====" << std::endl;
    std::cout << "  1. average access time:\t\t\t" << accessTimeL1 << std::endl;
    std::cout << "  2. energy-delay product:\t\t\t" << accessTimeVC << std::endl;
    std::cout << "  3. total area:\t\t\t\t" << area << std::endl;


    }



}

else {


    CacheStructure cacheL1 = CacheStructure(blockSize, cacheSizeL1, associativityL1);
    CacheStructure cacheL2 = CacheStructure(blockSize, cacheSizeL2, associativityL2);
    FullyAssociativeCache cacheVC = FullyAssociativeCache(blockSize, cacheLinesVC);

    std::string _dirtyTag_;

    std::string address, operation;

    int totSwaps = 0;
    int VCMisses = 0;
    int totSwapReq = 0;
    int totAccess = 0;
    int Wb = 0;

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
            auto [hitVC, blockIndexVC] = cacheVC.checkHitOrMiss(address);

            if (operation == "r") {
                
                if (hitL1) {
                    cacheL1.accessCache(address, "r", true, blockIndexL1, false);
                }

                else if (!hitL1 && hitVC) {
                    cacheL1.accessCache(address, "r", false, blockIndexL1, true);
                    std::string addressVC = cacheL1.get_swap_address();
                    bool dirtyVC = cacheL1.get_swap_dirty();
                    cacheVC.allocate(addressVC, dirtyVC, true);

                    // _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    // if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);} 

                    totSwaps++;
                }

                else if (!hitL1 && !hitVC) {
                    cacheL1.accessCache(address, "r", false, blockIndexL1, true);

                    std::string addressVC = cacheL1.get_swap_address();
                    bool dirtyVC = cacheL1.get_swap_dirty();
                    cacheVC.allocate(addressVC, dirtyVC, false);

                    cacheL2.accessCache(address, "r", hitL2, blockIndexL2, false);

                    _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}

                    _dirtyTag_ = cacheVC.get_dirty_tag_address();

                    if (cacheVC.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}



                }

            }

            else {
                if (hitL1) {
                    cacheL1.accessCache(address, "w", true, blockIndexL1, false);


                }

                else if (!hitL1 && hitVC) {
                    cacheL1.accessCache(address, "w", false, blockIndexL1, true);
                    std::string addressVC = cacheL1.get_swap_address();
                    bool dirtyVC = cacheL1.get_swap_dirty();
                    cacheVC.allocate(addressVC, dirtyVC, true);

                    totSwaps++;
                }

                else if (!hitL1 && !hitVC){
                    cacheL1.accessCache(address, "w", false, blockIndexL1, true);
                    std::string addressVC = cacheL1.get_swap_address();
                    bool dirtyVC = cacheL1.get_swap_dirty();

                    cacheVC.allocate(addressVC, dirtyVC, false);

                    cacheL2.accessCache(address, "r", hitL2, blockIndexL2, false);

                    _dirtyTag_ = cacheL1.get_dirty_tag_address();

                    if (cacheL1.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}

                    _dirtyTag_ = cacheVC.get_dirty_tag_address();

                    if (cacheVC.get_dirty_hit()) {cacheL2.dirtyWrite(_dirtyTag_);}





                }
            }


        }

     _file_.close();

    }

    float combinedMissrateL1 = (static_cast<float> (cacheL1.get_read_misses() + cacheL1.get_write_misses()) / (cacheL1.get_read_count() + cacheL1.get_write_count()));
    float combinedMissrateL2 = (static_cast<float> (cacheL2.get_read_misses() + cacheL2.get_write_misses()) / (cacheL2.get_read_count() + cacheL2.get_write_count()));
    int total_memory_traffic = cacheL2.get_read_misses() + cacheL2.get_write_misses() + cacheL2.get_dirty_writes();
    float missRateL2 =  static_cast<float> (cacheL2.get_read_misses() / cacheL2.get_read_count());
    float accessTimeL1, energyL1, areaL1;
    float accessTimeL2, energyL2, areaL2;
    float accessTimeVC, energyVC, areaVC;
    int VCSize = blockSize * cacheLinesVC;
    int AssociativityVC = (VCSize / blockSize);
    int resultL1 = get_cacti_results(cacheSizeL1, blockSize, associativityL1, &accessTimeL1, &energyL1, &areaL1);
    int resultL2 = get_cacti_results(cacheSizeL2, blockSize, associativityL2, &accessTimeL2, &energyL2, &areaL2);
    int resultVC = get_cacti_results(VCSize, blockSize, AssociativityVC, &accessTimeVC, &energyVC, &areaVC);


    if (!resultL1 && !resultL2 && !resultVC) {

    float AATL2 = accessTimeL2 + static_cast<float>(combinedMissrateL2) * mainMemoryPenalty;
    float AAT = accessTimeL1 + static_cast<float>(combinedMissrateL1) * AATL2;
    float energyDelayProduct = (energyL1* static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_read_count() + cacheL1.get_write_count() + cacheL1.get_write_misses()) + 0.05* static_cast<float>(cacheL2.get_read_misses() + cacheL2.get_write_misses() + cacheL2.get_dirty_writes()) + energyL2 * static_cast<float>(cacheL1.get_read_misses() + cacheL1.get_read_count() + cacheL1.get_write_count() + cacheL1.get_write_misses()))*AAT * 100000.0f;
    float area = areaL1 + areaL2;
    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << "  L1_SIZE:\t\t" << cacheSizeL1 << std::endl;
    std::cout << "  L1_ASSOC:\t\t" << associativityL1 << std::endl;
    std::cout << "  L1_BLOCKSIZE:\t\t" << blockSize << std::endl;
    std::cout << "  VC_NUM_BLOCKS:\t" << cacheLinesVC << std::endl;
    std::cout << "  L2_SIZE:\t\t" << cacheSizeL2 << std::endl;
    std::cout << "  L2_ASSOC:\t\t" << associativityL2 << std::endl;
    std::cout << "  trace_file:\t\t" << file_name << std::endl;
    std::cout << std::endl;

    std::cout << "===== L1 contents =====" << std::endl;
    cacheL1.memory();
    std::cout << std::endl;

    std::cout << "===== L2 contents =====" << std::endl;
    cacheL2.memory();
    std::cout << std::endl;


    std::cout << "===== Simulation results (raw) =====" << std::endl;
    std::cout << "  a. number of L1 reads:\t\t\t\t" << cacheL1.get_read_count() << std::endl;
    std::cout << "  b. number of L1 read misses:\t\t\t\t" << cacheL1.get_read_misses() << std::endl;
    std::cout << "  c. number of L1 writes:\t\t\t\t" << cacheL1.get_write_count() << std::endl;
    std::cout << "  d. number of L1 write misses:\t\t\t\t" << cacheL1.get_write_misses() << std::endl;
    std::cout << "  e. number of swap requests:\t\t\t\t0" << std::endl;
    std::cout << "  f. swap request rate:\t\t\t\t\t0.0000" << std::endl;
    std::cout << "  g. number of swaps:\t\t\t\t\t0" << std::endl;
    std::cout << "  h. combined L1+VC miss rate:\t\t\t\t"  << combinedMissrateL1 << std::endl;
    std::cout << "  i. number writebacks from L1/VC:\t\t\t" << cacheL1.get_dirty_writes() << std::endl;
    std::cout << "  j. number of L2 reads:\t\t\t\t" << cacheL2.get_read_count() << std::endl;
    std::cout << "  k. number of L2 read misses:\t\t\t\t" << cacheL2.get_read_misses() << std::endl;
    std::cout << "  l. number of L2 writes:\t\t\t\t" << cacheL2.get_write_count() << std::endl;
    std::cout << "  m. number of L2 write misses:\t\t\t\t" << cacheL2.get_write_misses() << std::endl;
    std::cout << "  n. L2 miss rate:\t\t\t\t\t" << missRateL2 << std::endl;
    std::cout << "  o. number of writebacks from L2:\t\t\t" << cacheL2.get_dirty_writes() << std::endl;
    std::cout << "  p. total memory traffic:\t\t\t\t" << total_memory_traffic << std::endl;

    std::cout << std::endl;

    std::cout << "===== Simulation results (performance) =====" << std::endl;
    std::cout << "  1. average access time:\t\t\t" << AAT << std::endl;
    std::cout << "  2. energy-delay product:\t\t\t" << energyDelayProduct << std::endl;
    std::cout << "  3. total area:\t\t\t\t" << area << std::endl;


        }
}

    return 0;


}