#include <optional>
#include <vector>
#include <iostream>
#include <cstdint>
#include "broadcast.h"

std::optional<std::vector<int64_t>> broadcastShape(const std::vector<int64_t> &a, const std::vector<int64_t> &b)
{

    for (size_t i = 0; i < a.size(); i++)
    {
        if (a[i] <= 0)
        {
            return std::nullopt;
        }
    }

    for (size_t i = 0; i < b.size(); i++)
    {
        if (b[i] <= 0)
        {
            return std::nullopt;
        }
    }

    std::vector<int64_t> higherdimensional = a.size() > b.size() ? a : b;
    std::vector<int64_t> lowerdimensional = a.size() > b.size() ? b : a;

    int diff = higherdimensional.size() - lowerdimensional.size();

    std::vector<int64_t> final_shape(higherdimensional.size());
    std::optional<std::vector<int64_t>> broadcast_output;

    for (int i = 0; i < diff; i++)
    {
        final_shape[i] = higherdimensional[i];
    }

    for (size_t i = diff; i < higherdimensional.size(); i++)
    {
        if (higherdimensional[i] == lowerdimensional[i - diff] || higherdimensional[i] == 1 || lowerdimensional[i - diff] == 1)
        {
            final_shape[i] = (higherdimensional[i] > lowerdimensional[i - diff] ? higherdimensional[i] : lowerdimensional[i - diff]);
        }
        else
        {
            return std::nullopt;
        }
    }

    broadcast_output = final_shape;
    return broadcast_output;
}

// int main()
// {
//     auto result = broadcastShape({1, 32, 112, 112}, {32,1,1});

//     if (result.has_value())
//     {
//         for (size_t i = 0; i < result->size(); i++)
//         {
//             std::cout << result->at(i) << " ";
//         }
//         std::cout << std::endl;
//     }
// }
