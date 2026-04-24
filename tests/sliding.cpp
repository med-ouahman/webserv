
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <limits.h>
using vector = std::vector<int>;

/* SLIDING WINDOW ALGORITHM */

/* 1ST PROBLEM */

/* MAXIMUM SUBARRAY SUM OF LENGTH K */

int get_curr_sum(size_t i, vector& nums, size_t k ) {

    int sum(0);
    int j = 0;
    while (j < k) {
        j++;
        i++;
        sum += nums[i];
    }
    return sum;
}

int maximum_subarray_sum( vector& nums, size_t k ) {
    int sum = INT_MIN;

    int max_iter = nums.size() - k;
    
    if (max_iter < 0) {
        throw std::runtime_error("Expected subarray size to be >= array size");
    }

    int iters = 0;
    for ( size_t i = 0; i < nums.size(); ++i ) {
        if (iters >= max_iter) {
            break;
        }
      int curr_sum = get_curr_sum(i, nums, k);
      if (curr_sum > sum) {
        sum = curr_sum;
      } 
      iters++;
    }

    return sum;
}












int main( int argc, char** argv ) {
    vector nums;

    for ( size_t i = 1; i < argc; ++i) {
        try {

            nums.push_back(std::stoi(argv[i]));
        } catch (std::runtime_error& err ){
            std::cerr << err.what() << "\n";
            return 1;
        }

    }
    try {
        int m = maximum_subarray_sum(nums, 4);
        std::cout << "Max subarray sum: " << m << "\n";
        
    } catch( std::runtime_error& err ){
        std::cerr << err.what( ) << "\n";
    }

    return 0;
}