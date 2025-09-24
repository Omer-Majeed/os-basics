/*
 *  Author: Omer Majeed <omer.majeed734@gmail.com>
 */

#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>

using namespace std;

typedef struct alignas(64) param {
    volatile bool init_flag = false;
    volatile bool terminate = false;
    volatile bool terminated = false;
    int thread_index = -1;
} param;

int counter = 0;
constexpr int T = 8, INC = 100'000;

void* worker(void* p) {
    param *p_param = (param*) p;
    cout << "Initialized thread[" << p_param->thread_index << "] = " << pthread_self() << endl;
    p_param->init_flag = true;
    for (int i=0;i<INC;i++) counter++;
    while (p_param->terminate == false) {
        // usually this while is the actual place where worker threads do their actual work
        cout << "thread[" << p_param->thread_index << "] = " << pthread_self() << " Doing work!" << endl;
        usleep(2000000); // 2 sec
    }
    cout << "Exiting thread[" << p_param->thread_index << "] = " << pthread_self() << endl;
    p_param->terminated = true;
    return nullptr;
}

int main() {

    std::vector<pthread_t> thr(T);
    std::vector<param> params(T); 
    
    for (int t = 0; t < T; ++t) {
        params[t].thread_index = t;
        pthread_create(&thr[t], nullptr, worker, &params[t]);

        while(params[t].init_flag == false);
    }
    
    for (int i=0;i<INC;i++) counter++;

    usleep(10000000); // sleep for 10 seconds
    cout << "\nAbout to terminate all threads" << endl;
    for (int t = 0; t < T; ++t) {
        params[t].terminate = true;

        while (params[t].terminated == false);
    }

    std::cout << "Expected " << 1LL * (T + 1) * INC << " got " << counter << "\n";
}
