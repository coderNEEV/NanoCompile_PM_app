#include "broadcast.h"
#include <chrono>

// Tester

// {1,99,99,1....99} (given rank)
std::vector<int64_t> test_generator(int rank)
{
    std::vector<int64_t> shape(rank);

    for (int i = 0; i < rank - 1; i++)
    {
        shape[i] = rand() % 2 ? 99 : 1;
    }
    shape[rank - 1] = 99; // Can be used for shape extension testings
}

int main(void)
{
    // Same rank tester
    int N = 4;
    std::vector<int64_t> a = test_generator(N);
    std::vector<int64_t> b = test_generator(N);

    auto start =
        std::chrono::high_resolution_clock::now();
    
        for (int i = 0; i < 1000; i++)
        {
            auto shape = broadcastShape(a, b);
        }
        
    auto end =
        std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<
            std::chrono::nanoseconds>(
            end - start);
    auto data = duration/1000;

    // Different rank tester
    int N2 = 8;
    std::vector<int64_t> a = test_generator(4);
    std::vector<int64_t> b = test_generator(N2);
    auto start =
        std::chrono::high_resolution_clock::now();
    
        for (int i = 0; i < 1000; i++)
        {
            auto shape = broadcastShape(a, b);
        }
        
    auto end =
        std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<
            std::chrono::nanoseconds>(
            end - start);
    auto data_diff = duration/1000;

}
