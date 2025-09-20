/*
 *  Author: Omer Majeed <omer.majeed734@gmail.com>
 */

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define TEST_MESSAGES   1000000

int main() {
    int p1[2]; // parent to child
    int p2[2]; // child to parent

    pipe(p1);
    pipe(p2);

    printf("Parent Starting\n");

    int pid = fork();
    if (pid == 0) { // child
        printf("Child Running %d message exchanges test\n", TEST_MESSAGES);

        close(p1[0]);
        close(p2[1]);

        int pong = -1;

        clock_t start_time = clock();
        for (int ping = 0; ping < TEST_MESSAGES; ++ping) {
            write(p1[1], &ping, sizeof(ping));
            read(p2[0], &pong, sizeof(pong));
            
            assert (ping == pong);
        }
        clock_t end_time = clock();

        double time_taken = ((double)(end_time - start_time)) / (CLOCKS_PER_SEC * TEST_MESSAGES);
        printf("AvgTime taken by %d message exchanges = %f seconds\n", TEST_MESSAGES, time_taken);
        int rate = CLOCKS_PER_SEC / ((double)(end_time - start_time) / TEST_MESSAGES);
        printf("Rate= %d / second\n", rate);
    } else if (pid > 0) { // parent
        int ping = -1;

        close(p1[1]);
        close(p2[0]);

        while (waitpid(pid, NULL, WNOHANG) == 0) {
            if (read(p1[0], &ping, sizeof(ping)) <= 0) {
                break;
            }
            write(p2[1], &ping, sizeof(ping));
        }
        printf("Parent Ending\n");
    } else {
        printf("Error creating child\n");
    }

    return 0;
}