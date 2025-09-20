/*
 *  Author: Omer Majeed <omer.majeed734@gmail.com>
 */

#include <iostream>
#include <vector>
#include <pthread.h>

int counter = 0;
constexpr int T = 8, INC = 100'000;

void* worker(void* p) {
    for (int i=0;i<INC;i++) counter++;
    return nullptr;
}

int main() {

    std::vector<pthread_t> thr(T);
    
    for (int t = 0; t < T; ++t) {
        pthread_create(&thr[t], nullptr, worker, NULL);
    }
    
    for (int i=0;i<INC;i++) counter++;
    
    for (int t = 0; t < T; ++t) {
        pthread_join(thr[t], nullptr);
    }

    std::cout << "Expected " << 1LL * (T + 1) * INC << " got " << counter << "\n";
}
