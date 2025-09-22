/*
 *  Author: Omer Majeed <omer.majeed734@gmail.com>
 */

#include <atomic>
#include <thread>
#include <iostream>

struct Node { int data; Node* next; };
std::atomic<Node*> head{nullptr};

void writer() {
    Node* n = new Node{42, nullptr};
    head.store(n, std::memory_order_release);
}

void reader() {
    Node* n = head.load(std::memory_order_consume);
    if (n) {
        std::cout << "Read node->data = " << n->data << "\n";
    } else {
        std::cout << "No node yet\n";
    }
}

int main() {
    std::thread t1(writer), t2(reader);
    t1.join(); t2.join();
}
