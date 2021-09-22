/*
 * Copyright (c) 2018-2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int pipefds[2];

int main(int, char**)
{
    if(pipe2(pipefds,O_NONBLOCK) != 0) {
        printf("FAIL, could not create nonblocking pipe\n");
        return 1;
    }

    pthread_t tid;
    pthread_create(
        &tid, nullptr, [](void*) -> void* {
            sleep(1);
            printf("Second thread doing a write to pipe...\n");
            if(write(pipefds[1], "ABCD", 4) != 4) {
                printf("FAIL, could not write to pipe\n");
                return nullptr;
            }
            printf("Second thread success\n");
            pthread_exit(nullptr);
            return nullptr;
        },
        nullptr);

    ssize_t nread = 0;
    for (int n=0; n<6; n++) {
        usleep(250e3);
        printf("First thread doing a read from pipe...\n");
        char buffer[16];
        nread = read(pipefds[0], buffer, sizeof(buffer));
        if (nread != 4) {
            continue;
        }
        printf("First thread success\n");
        break;
    }
    if (nread != 4) {
        printf("FAIL, read %zd bytes from pipe\n", nread);
        return 1;
    }

    printf("PASS\n");

    return 0;
}
