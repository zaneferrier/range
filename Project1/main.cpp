#include "range_copy.hpp"
#include "range_map.hpp"
#include "range_filter.hpp"
#include "range_stride.hpp"
#include "range_unique.hpp"
#include "range_reverse.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace adaptor;

int main()
{
    std::vector<int> x = { 1,2,3,4,5,6,7,8,9,10 };

    auto rng = x | adaptor::slice(1, 5);

    for (auto value : rng) {
        std::cout << value << ", ";
    }
    std::cout << '\n';

    auto mp = x | adaptor::map([](int x) { return x * 2; })
                | adaptor::slice(1, 5);

    for (auto value : mp) {
        std::cout << value << ", ";
    }
    std::cout << '\n';

    auto filt = x | adaptor::filter([](int x) { return x % 2 == 1; })
                  | adaptor::map([](int x) { return x * 2; })
                  | adaptor::unique();

    for (auto value : filt) {
       std::cout << value << ", ";
    }
    std::cout << '\n';

    auto str = x | adaptor::stride(6);

    for(auto value : str) {
        std::cout << value << ", ";
    }
    std::cout << '\n';

    std::vector<int> y = { 1,1,2,2,3,3,4,4,5,6,6 };
    auto uq = y | adaptor::unique();
    auto uq2 = std::vector<int>(10, 0) | adaptor::unique();
 
    for(auto v : uq) {
        std::cout << v << ", ";
    }
    std::cout << '\n';

    for(auto v : uq2) {
        std::cout << v << ", ";
    }
    std::cout << '\n';

    auto rev = x | adaptor::reverse();
    for(auto v : rev) {
        std::cout << v << ", ";
    }
    std::cout << '\n';
}
