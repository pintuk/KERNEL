#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define MEM_CHUNK_MB 256
#define IO_FILE "/tmp/psi_io_stress.bin"
#define CPU_THREADS 2
#define MEM_THREADS 2
#define IO_THREADS 2

static volatile int running = 1;

static void set_thread_name(const char *name)
{
    prctl(PR_SET_NAME, name, 0, 0, 0);
}

/* ------------------------------------------------ */
/* CPU PRESSURE THREAD */
/* ------------------------------------------------ */

void *cpu_pressure_thread(void *arg)
{
    char name[16];
    int id = (intptr_t)arg;

    snprintf(name, sizeof(name), "cpu_hog_%d", id);
    set_thread_name(name);

    volatile double x = 0;

    while (running) {
        for (int i = 0; i < 10000000; i++)
            x += sqrt(i * 123.456);
    }

    return NULL;
}

/* ------------------------------------------------ */
/* MEMORY PRESSURE THREAD */
/* ------------------------------------------------ */

void *memory_pressure_thread(void *arg)
{
    char name[16];
    int id = (intptr_t)arg;

    snprintf(name, sizeof(name), "mem_hog_%d", id);
    set_thread_name(name);

    while (running) {

        size_t sz = MEM_CHUNK_MB * 1024 * 1024;

        char *buf = malloc(sz);

        if (!buf) {
            usleep(100000);
            continue;
        }

        for (size_t i = 0; i < sz; i += 4096)
            buf[i] = (char)i;

        usleep(500000);

        free(buf);

        usleep(100000);
    }

    return NULL;
}

/* ------------------------------------------------ */
/* IO PRESSURE THREAD */
/* ------------------------------------------------ */

void *io_pressure_thread(void *arg)
{
    char name[16];
    int id = (intptr_t)arg;

    snprintf(name, sizeof(name), "io_hog_%d", id);
    set_thread_name(name);

    int fd = open(IO_FILE, O_CREAT | O_RDWR | O_SYNC, 0644);

    if (fd < 0) {
        perror("open");
        return NULL;
    }

    char *buf = malloc(4096);
    memset(buf, 'A', 4096);

    while (running) {

        for (int i = 0; i < 100000; i++) {
            write(fd, buf, 4096);
        }

        fsync(fd);
        lseek(fd, 0, SEEK_SET);
    }

    close(fd);
    free(buf);

    return NULL;
}

/* ------------------------------------------------ */
/* MAIN */
/* ------------------------------------------------ */

int main(void)
{
    pthread_t threads[CPU_THREADS + MEM_THREADS + IO_THREADS];
    int idx = 0;

    printf("Starting PSI workload simulator...\n");

    for (int i = 0; i < CPU_THREADS; i++)
        pthread_create(&threads[idx++], NULL,
                       cpu_pressure_thread,
                       (void *)(intptr_t)i);

    for (int i = 0; i < MEM_THREADS; i++)
        pthread_create(&threads[idx++], NULL,
                       memory_pressure_thread,
                       (void *)(intptr_t)i);

    for (int i = 0; i < IO_THREADS; i++)
        pthread_create(&threads[idx++], NULL,
                       io_pressure_thread,
                       (void *)(intptr_t)i);

    printf("Workloads active. Press Ctrl+C to stop.\n");

    while (1)
        sleep(1);

    return 0;
}

