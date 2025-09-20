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
        close(0);   // 0 is usually for console output, close 0 so that it is available for next step
        dup(p2[0]); // right after closing 0, when we do duplicate of p2[0] it uses first fd available which is 0 now, since we closed 0 above
        close(1);   // 1 is usually for console input, close 1 so that it is available for next step
        dup(p1[1]); // right after closing 1, when we do duplicate of p1[1] it uses first fd available which is 1 now, since we closed 1 above

        close(p1[0]);
        close(p1[1]);   // this is copied to fd 1
        close(p2[0]);   // this is copied to fd 0
        close(p2[1]);

        int pong = -1;

        /*
         *  Now that we have closed fd 0, we can not use printf, it would use fd 0
         *  That we already are using to read from parent.
         *  If you uncomment this following print, you should see assert failure
         *  What you can do is instead use fd 2 to put printf, fd 2 is to output errors to console
         *  (0 for console output, 1 for console input, 2 is for console error output)
         */        
        
        // printf("Child Running %d message exchanges test\n", TEST_MESSAGES);              // Both of these commented lines are same, use stdout = 0 fd
        // fprintf(stdout, "Child Running %d message exchanges test\n", TEST_MESSAGES);

        fprintf(stderr, "Child Running %d message exchanges test\n", TEST_MESSAGES);        // stderr uses fd = 1

        for (int ping = 0; ping < TEST_MESSAGES; ++ping) {
            write(1, &ping, sizeof(ping));
            read(0, &pong, sizeof(pong));
            assert (ping == pong);
        }
    } else if (pid > 0) { // parent
        int ping = -1;

        close(p1[1]);
        close(p2[0]);

        clock_t start_time = clock();
        while (waitpid(pid, NULL, WNOHANG) == 0) {
            if (read(p1[0], &ping, sizeof(ping)) <= 0) {
                break;
            }
            write(p2[1], &ping, sizeof(ping));
        }
        clock_t end_time = clock();

        double time_taken = ((double)(end_time - start_time)) / (CLOCKS_PER_SEC * TEST_MESSAGES);
        printf("AvgTime taken by %d message exchanges = %f seconds\n", TEST_MESSAGES, time_taken);
        int rate = CLOCKS_PER_SEC / ((double)(end_time - start_time) / TEST_MESSAGES);
        printf("Rate= %d / second\n", rate);
    } else {
        printf("Error creating child\n");
    }

    return 0;
}