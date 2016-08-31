#include "range_copy.hpp"
#include "range_map.hpp"
#include "range_filter.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace adaptor;

int main()
{
    std::vector<int> x = { 1,2,3,4,5,6,7,8,9,10 };

    auto rng = x | adaptor::slice(1, 5);

    for (auto value : rng) {
        std::cout << value << '\n';
    }

    auto mp = x | adaptor::map([](int x) { return x * 2; })
                | adaptor::slice(1, 5);

    for (auto value : mp) {
        std::cout << value << '\n';
    }

    auto filt = x | adaptor::filter([](int x) { return x % 2 == 1; })
                  | adaptor::map([](int x) { return x * 2; });

    for (auto value : filt) {
       std::cout << value << '\n';
    }
}
