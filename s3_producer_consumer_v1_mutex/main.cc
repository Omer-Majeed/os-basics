/*
 *  Author: Omer Majeed <omer.majeed734@gmail.com>
 */

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

typedef struct alignas(64) log_data {
    char data[1024];
    int thread_index;
    uint64_t tid;
} log_data;

class Queue {
    std::queue<log_data *> m_queue;
    std::mutex m_mutex;
    public:
    Queue() {}
    void Push(log_data *_data);
    log_data * Pop();
    void Lock();
    void Unlock();
    bool IsEmpty() {
        return m_queue.empty();
    }
};

void Queue::Push(log_data *_data) {
    m_queue.push(_data);
}
log_data * Queue::Pop() {
    log_data * ret = NULL;
    if (m_queue.empty() == false) {
        ret = m_queue.front();
        m_queue.pop();
    }
    return ret;
}
void Queue::Lock() {
    m_mutex.lock();
}
void Queue::Unlock() {
    m_mutex.unlock();
}

typedef struct alignas(64) param {
    volatile bool init_flag = false;
    volatile bool terminate = false;
    volatile bool terminated = false;
    int thread_index = -1;
    Queue *p_queue;
    Queue *p_feedback_queue;
} param;

int counter = 0;
constexpr int T = 8, INC = 100'000;

#define TEST_MAX_MESSAGES    10

void* producer(void* p) {
    param *p_param = (param*) p;
    int thread_idx = p_param->thread_index;
    uint64_t tid = (uint64_t) pthread_self();
    Queue *p_queue = p_param->p_queue;

    cout << "Initialized Producer thread[" << p_param->thread_index << "] = " << pthread_self() << " as_uint64_t = " << tid << endl;

    p_param->init_flag = true;
    int i = 0;

    while (p_param->terminate == false) {
        for (; i < TEST_MAX_MESSAGES; ++i) {
            log_data *new_log = new log_data();
            memset (new_log, 0,  sizeof(log_data));
            new_log->thread_index = thread_idx;
            new_log->tid = tid;
            snprintf(new_log->data, 1024, "Producer Thread-Index[%d] tid:%llx Log Data Count=%d\n", thread_idx, tid, i);

            p_queue->Lock();
            p_queue->Push(new_log);
            p_queue->Unlock();
        }
    }

    cout << "Exiting Producer thread[" << p_param->thread_index << "] = " << pthread_self() << endl;
    p_param->terminated = true;
    return nullptr;
}

void* consumer(void* p) {
    param *p_param = (param*) p;
    int thread_idx = p_param->thread_index;
    uint64_t tid = (uint64_t) pthread_self();
    Queue *p_queue = p_param->p_queue;
    Queue *p_feedback_queue = p_param->p_feedback_queue;

    cout << "Initialized Consumer thread[" << thread_idx << "] = " << pthread_self() << " as_uint64_t = " << tid << endl;

    p_param->init_flag = true;

    while (p_param->terminate == false) {
        if (p_queue->IsEmpty()) {
            usleep(10);
            continue;
        }
        p_queue->Lock();
        log_data * data = p_queue->Pop();
        p_queue->Unlock();

        p_feedback_queue->Lock();
        p_feedback_queue->Push(data);
        p_feedback_queue->Unlock();
    }

    cout << "Exiting Consumer thread[" << p_param->thread_index << "] = " << pthread_self() << endl;
    p_param->terminated = true;
    return nullptr;
}

int main() {

    pthread_t cons_thr;
    param cons_param;
    std::vector<pthread_t> prod_thr(T);
    std::vector<param> params(T);
    Queue out_queue;
    Queue in_queue;

    cons_param.thread_index = 0;
    cons_param.p_queue = &out_queue;
    cons_param.p_feedback_queue = &in_queue;
    pthread_create(&cons_thr, nullptr, consumer, &cons_param);

    while(cons_param.init_flag == false);

    
    for (int t = 0; t < T; ++t) {
        params[t].thread_index = t;
        params[t].p_queue = &out_queue;
        pthread_create(&prod_thr[t], nullptr, producer, &params[t]);

        while(params[t].init_flag == false);
    }
    

    int expected = TEST_MAX_MESSAGES * T;
    int rsp_count = 0;
    while (expected > 0) {
        if (in_queue.IsEmpty()) {
            usleep(10);
            continue;
        }
        in_queue.Lock();
        log_data *data = in_queue.Pop();
        assert (data);
        in_queue.Unlock();

        cout << "Message:" << rsp_count << " Received data Produced by Thread-Index:" << data->thread_index << " tid:" << data->tid << " message:" << data->data << endl;
        delete data;
        --expected;
        ++rsp_count;
    }
    
    cout << "\nAbout to terminate all threads" << endl;
    cons_param.terminate = true;
    while (cons_param.terminated == false);

    for (int t = 0; t < T; ++t) {
        params[t].terminate = true;

        while (params[t].terminated == false);
    }

    cout << "\nExiting Main Process" << endl;
    return 0;
}
