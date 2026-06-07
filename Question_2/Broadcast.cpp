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

    for (int i = 0; i < diff; i++)
    {
        lowerdimensional.insert(lowerdimensional.begin(), (int64_t)1);
    }

    bool valid = true;

    std::vector<int64_t> final_shape;
    std::optional<std::vector<int64_t>> broadcast_output;

    for (int i = 0; i < higherdimensional.size(); i++)
    {
        if (higherdimensional[i] == lowerdimensional[i] || higherdimensional[i] == 1 || lowerdimensional[i] == 1)
        {
            final_shape.push_back(higherdimensional[i] > lowerdimensional[i] ? higherdimensional[i] : lowerdimensional[i]);
        }
        else
        {
            valid = false;
            break;
        }
    }

    if (valid)
    {
        broadcast_output = final_shape;
        return broadcast_output;
    }
    else
    {
        return std::nullopt;
    }
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