#include "headers.h"

void *production(void *args);
void *vaccination(void *args);
void *insertion(void *args);

int main()
{
    fflush(stdout);
    printf("%sEnter the number of companies, zones and students:%s\n", BOLD_BLUE, RESET);
    for (int i = 0; i < 3; i++)
    {
        scanf("%d", &number_of[i]);
        if (number_of[i] == 0)
        {
            fflush(stdout);
            printf("%sOnly non-zero positive integers are expected%s\n", BOLD_RED, RESET);
            exit(0);
        }
    }

    // initialise companies
    fflush(stdout);
    printf("%sEnter success rates for companies:%s\n", BOLD_BLUE, RESET);
    for (int i = 0; i < number_of[0]; i++)
    {
        companies[i] = malloc(sizeof(struct company_t));
        scanf("%f", &(companies[i]->rate));
        companies[i]->pending = 0;

        pthread_mutex_init(&companies[i]->lock, NULL);
        pthread_cond_init(&companies[i]->cond, NULL);
    }

    // initialise zones
    for (int i = 0; i < number_of[1]; i++)
    {
        zones[i] = malloc(sizeof(struct zone_t));
        zones[i]->comapny = -1;
        zones[i]->waiting = 0, zones[i]->empty = 0;

        pthread_mutex_init(&zones[i]->lock, NULL);
        pthread_cond_init(&zones[i]->cond, NULL);
    }

    // initialise students
    pthread_mutex_init(&lock_insertion, NULL);
    pthread_cond_init(&cond_insertion, NULL);

    for (int i = 0; i < number_of[2]; i++)
    {
        students[i] = malloc(sizeof(struct student_t));
        students[i]->zone = -1;
        students[i]->chances = 0, students[i]->rate = 0.0;

        pthread_mutex_init(&students[i]->lock, NULL);
        pthread_cond_init(&students[i]->cond, NULL);
    }
    roll = 0, number_of[3] = number_of[2];

    // create threads
    for (int i = 0; i < number_of[0]; i++)
    {
        struct index_t *node = malloc(sizeof(struct index_t));
        node->val = i;

        pthread_create(&companies[i]->thread, NULL, production, node);
    }
    for (int i = 0; i < number_of[1]; i++)
    {
        struct index_t *node = malloc(sizeof(struct index_t));
        node->val = i;

        pthread_create(&zones[i]->thread, NULL, vaccination, node);
    }
    for (int i = 0; i < number_of[2]; i++)
    {
        struct index_t *node = malloc(sizeof(struct index_t));
        node->val = i;

        pthread_create(&students[i]->thread, NULL, insertion, node);
    }

    // join threads
    for (int i = 0; i < number_of[0]; i++)
        pthread_join(companies[i]->thread, NULL);
    for (int i = 0; i < number_of[1]; i++)
        pthread_join(zones[i]->thread, NULL);
}

int randomize(int s, int n)
{
    return (rand() % (n - s + 1)) + s;
}

void *production(void *args)
{
    struct index_t *node = args;

    while (1)
    {
        // get randomized data for company
        pthread_mutex_lock(&companies[node->val]->lock);

        int BATCHES = randomize(MIN_COMPANY_BATCHES, MAX_COMAPNY_BATCHES), BATCH_INDEX = 0,
            BATCHES_UNSUPPLIED = BATCHES;
        companies[node->val]->pending = BATCHES;

        for (int i = 0; i < BATCHES; i++)
            companies[node->val]->batches[i] = randomize(MIN_BATCH_SIZE, MAX_BATCH_SIZE);

        fflush(stdout);
        printf("%sPharmaceutical Company %d has started making %d batches with success rate %f%s\n", YELLOW, node->val,
              BATCHES, companies[node->val]->rate, RESET);
        sleep(randomize(MIN_COMPANY_WAIT_TIME, MAX_COMPANY_WAIT_TIME));
        fflush(stdout);
        printf("%sPharmaceutical Company %d has prepared %d batches with success rate %f%s\n", GREEN, node->val, BATCHES,
              companies[node->val]->rate, RESET);

        pthread_mutex_unlock(&companies[node->val]->lock);

        // supply on demand
        while (companies[node->val]->pending > 0)
        {
            while (BATCHES_UNSUPPLIED > 0)
            {
                for (int i = 0; i < number_of[1] && BATCHES_UNSUPPLIED > 0; i++)
                {
                    if (BATCHES_UNSUPPLIED > 0 && zones[i]->waiting == 1)
                    {
                        pthread_mutex_lock(&zones[i]->lock);

                        if (zones[i]->waiting == 1)
                        {
                            zones[i]->comapny = node->val, zones[i]->waiting = 0,
                            zones[i]->empty = companies[node->val]->batches[BATCH_INDEX];
                            BATCH_INDEX++, BATCHES_UNSUPPLIED--;

                            pthread_cond_signal(&zones[i]->cond);
                        }

                        pthread_mutex_unlock(&zones[i]->lock);
                    }
                }
            }
        }
    }
}

int min(int x, int y)
{
    if (x < y)
        return x;
    return y;
}

void *vaccination(void *args)
{
    struct index_t *node = args;

    while (1)
    {
        // get supply
        pthread_mutex_lock(&zones[node->val]->lock);

        while (zones[node->val]->comapny == -1)
        {
            zones[node->val]->waiting = 1;
            pthread_cond_wait(&zones[node->val]->cond, &zones[node->val]->lock);
        }

        int EMPTY = zones[node->val]->empty, COMPANY = zones[node->val]->comapny;
        zones[node->val]->waiting = 0;

        fflush(stdout);
        printf("%sPharmaceutical Company %d has provided %d vaccines to Vaccination Zone %d%s\n", CYAN, COMPANY, EMPTY,
              node->val, RESET);
        pthread_mutex_unlock(&zones[node->val]->lock);

        // vaccinate
        int SLOTS;
        while (EMPTY > 0)
        {
            if (EMPTY == 1)
                SLOTS = 1;
            else
                SLOTS = randomize(1, min(EMPTY, MAX_ZONE_SLOTS));
            fflush(stdout);
            printf("%sVaccination Zone %d has %d empty slots and is ready to vaccinate%s\n", GREEN, node->val, SLOTS,
                  RESET);

            pthread_mutex_lock(&lock_insertion), roll++;
            pthread_mutex_unlock(&lock_insertion);

            fflush(stdout);
            printf("%sVaccination Zone %d has started vaccination phase%s\n", CYAN, node->val, RESET);

            int SENT = 0;
            for (int i = 0; i < number_of[3] && SENT < SLOTS; i++)
            {
                if (SENT < SLOTS && students[i]->zone == -1)
                {
                    pthread_mutex_lock(&students[i]->lock);
                    if (students[i]->zone == -1)
                    {
                        fflush(stdout);
                        printf("%sStudent %d has been assigned Vaccination Zone %d%s\n", GREEN, i, node->val, RESET);
                        students[i]->zone = node->val, students[i]->rate = companies[zones[node->val]->comapny]->rate;
                        pthread_cond_signal(&students[i]->cond);

                        SENT++;
                    }
                    pthread_mutex_unlock(&students[i]->lock);
                }
            }
            fflush(stdout);
            printf(
                "%sVaccination Zone %d has ended vaccination phase with %d student trials and has %d vaccines left%s\n",
                YELLOW, node->val, SENT, EMPTY, RESET);

            pthread_mutex_lock(&lock_insertion), roll--;
            pthread_mutex_unlock(&lock_insertion);

            if ((EMPTY -= SENT) == 0)
                break;
            else if (SLOTS == SENT)
                continue;

            zones[node->val]->waiting = 2;
            do
            {
                pthread_cond_wait(&zones[node->val]->cond, &zones[node->val]->lock);
            } while (zones[node->val]->waiting == 2);
        }

        fflush(stdout);
        printf("%sVaccination Zone %d has run out of vaccines%s\n", RED, node->val, RESET);
        zones[node->val]->comapny = -1, zones[node->val]->empty = 0, zones[node->val]->waiting = 1;

        pthread_mutex_lock(&companies[COMPANY]->lock);
        companies[COMPANY]->pending -= 1;
        pthread_mutex_unlock(&companies[COMPANY]->lock);
    }
}

void *insertion(void *args)
{
    struct index_t *node = args;

    while (students[node->val]->chances != 3)
    {
        while (students[node->val]->zone == -1)
        {
            for (int i = 0; i < number_of[1]; i++)
            {
                if (zones[i]->waiting == 2)
                {
                    pthread_mutex_lock(&zones[i]->lock);
                    if (zones[i]->waiting == 2)
                        zones[i]->waiting = 0, pthread_cond_signal(&zones[i]->cond);

                    pthread_mutex_unlock(&zones[i]->lock);
                }
            }
        }

        fflush(stdout);
        printf(
            "%sStudent %d has been vaccinated at vaccination zone %d with %f success rate. This was his %d chance.%s\n",
            YELLOW, node->val, students[node->val]->zone, students[node->val]->rate, (1 + students[node->val]->chances),
            RESET);

        float result = (float)(rand() % 100) / 100;
        if (result > students[node->val]->rate)
        {
            students[node->val]->chances++;
            fflush(stdout);
            printf("%sStudent %d has tested negative for antibodies%s\n", RED, node->val, RESET);

            if (students[node->val]->chances < 3)
            {
                students[node->val]->zone = -1;
                fflush(stdout);
                printf("%sStudent %d is waiting for a zone%s\n", BLUE, node->val, RESET);
                
                while (roll > 0)
                    continue;
            }
            else
            {
                fflush(stdout);
                printf("%sStudent %d is sent back home.%s\n", RED, node->val, RESET);

                if ((--number_of[2]) == 0)
                {
                    fflush(stdout);
                    fflush(stdout);
                    printf("%sSIMULATION IS OVER%s\n", BOLD_MAGENTA, RESET);
                    fflush(stdout);
                    exit(0);
                }
                pthread_exit(0);
            }
        }
        else
        {
            fflush(stdout);
            printf("%sStudent %d has tested positive for antibodies%s\n", BOLD_GREEN, node->val, RESET);
            if ((--number_of[2]) == 0)
            {
                fflush(stdout);
                fflush(stdout);
                printf("%sSIMULATION IS OVER%s\n", BOLD_MAGENTA, RESET);
                fflush(stdout);
                exit(0);
            }
            pthread_exit(0);
        }
    }
}