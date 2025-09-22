#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <cstdint>
#include <iomanip>

struct alignas(64) Padded {
    std::atomic<uint64_t> v{0};
    std::atomic<uint64_t> a1{0};
    std::atomic<uint64_t> a2{0};
    std::atomic<uint64_t> a3{0};
    std::atomic<uint64_t> a4{0};
    std::atomic<uint64_t> a5{0};
    std::atomic<uint64_t> a6{0};
    std::atomic<uint64_t> a7{0};
    std::atomic<uint64_t> a8{0};
    // static_assert(sizeof(std::atomic<uint64_t>) <= 64, "atomic too big for 64B line");
    // char pad[64 - sizeof(std::atomic<uint64_t>)];
};

struct Unpadded {
    std::atomic<uint64_t> v{0};
};

template<class Counter>
uint64_t run(Counter* arr, int T){
    std::vector<std::thread> th;
    th.reserve(T);
    for(int t=0;t<T;t++){
        th.emplace_back([&,t]{
            for (uint64_t i=0;i<100'000; ++i)
                arr[t].v.fetch_add(1, std::memory_order_relaxed);
        });
    }
    for (auto& x: th) x.join();
    uint64_t sum=0;
    for(int t=0;t<T;t++) sum += arr[t].v.load(std::memory_order_relaxed);
    return sum;
}

int main() {
    const int T = std::max(2u, std::thread::hardware_concurrency()); // at least 2
    std::cout << "Threads: " << T << "\n";
    

    // Unpadded test
    std::vector<Unpadded> unp(T);
    std::cout << "Sizeof(unp): " << sizeof(unp[0]) << "\n";
    auto t0 = std::chrono::steady_clock::now();
    uint64_t sum_unp = run(unp.data(), T);
    auto t1 = std::chrono::steady_clock::now();
    auto dur_unp = std::chrono::duration<double>(t1 - t0).count();

    // Padded test
    std::vector<Padded> pad(T);
    std::cout << "Sizeof(pad): " << sizeof(pad[0]) << "\n";
    auto t2 = std::chrono::steady_clock::now();
    uint64_t sum_pad = run(pad.data(), T);
    auto t3 = std::chrono::steady_clock::now();
    auto dur_pad = std::chrono::duration<double>(t3 - t2).count();

    // Each thread does 1e8 increments
    const uint64_t expected = static_cast<uint64_t>(T) * 100'000ull;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "[Unpadded]  sum=" << sum_unp << " (expected " << expected
              << "), time=" << dur_unp << "s\n";
    std::cout << "[Padded]    sum=" << sum_pad << " (expected " << expected
              << "), time=" << dur_pad << "s\n";
    std::cout << "Speedup (unpadded/padded): "
              << (dur_unp / dur_pad) << "x\n";
}
