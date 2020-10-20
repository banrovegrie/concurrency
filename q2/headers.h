#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <stddef.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX 10000
#define THREAD_MAX 256
#define MIN_COMPANY_WAIT_TIME 2
#define MAX_COMPANY_WAIT_TIME 5
#define MIN_COMPANY_BATCHES 1
#define MAX_COMAPNY_BATCHES 5
#define MIN_BATCH_SIZE 10
#define MAX_BATCH_SIZE 20
#define MAX_ZONE_SLOTS 8
#define MAX_CHANCES_PER_STUDENT 3

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define GREEN "\033[0;32m"
#define BOLD_GREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define BOLD_YELLOW "\033[01;33m"
#define BLUE "\033[0;34m"
#define BOLD_BLUE "\033[1;34m"
#define MAGENTA "\033[0;35m"
#define BOLD_MAGENTA "\033[1;35m"
#define CYAN "\033[0;36m"
#define BOLD_CYAN "\033[1;36m"
#define RESET "\033[0m"

struct company_t
{
    int pending, batches[MAX_COMAPNY_BATCHES];
    float rate;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t thread;
} * companies[THREAD_MAX];

struct zone_t
{
    int comapny, empty, waiting;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t thread;
} * zones[THREAD_MAX];

struct student_t
{
    int chances, zone;
    float rate;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t thread;
} * students[THREAD_MAX];

struct index_t
{
    int val;
};

extern int err;

// number of companies, zones, students and initial number of students
int number_of[] = {0, 0, 0, 0}, roll;

pthread_mutex_t lock_insertion;
pthread_cond_t cond_insertion;