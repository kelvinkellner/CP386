/*
 -------------------------------------
 File:    Question2.c
 Project: 190668940_190684430_a02_q21
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-19
 -------------------------------------
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* used to pass multiple arguments to calc functions */
typedef struct calc_args
{
    int arr[12]; // array of values
    int n;       // length of array
    /* calculation results */
    float avg;
    int max;
    int min;
    float med;
} CALC_ARGS;

/* aux function for creating pthreads */
pthread_t new_pthread(void *func, void *param);

/* math functions called by threads */
float calc_avg(CALC_ARGS *calc_args);
int calc_max(CALC_ARGS *calc_args);
int calc_min(CALC_ARGS *calc_args);
float calc_med(CALC_ARGS *calc_args);

/* -------------------------------- */
/*       ... Main Program ...       */
/* -------------------------------- */

void main(void)
{
    pthread_t workers[4]; // array for storing all thread identifiers
    int i;

    /* init arguments for calc functions */
    int arr[] = {98, 100, 102, 43, 43, 54, 64, 12, 43, 54, 11, 23};
    CALC_ARGS calc_args;
    calc_args.n = 12;
    for (i = 0; i < calc_args.n; i++)
    {
        calc_args.arr[i] = arr[i];
    }

    /* create workers and call apropriate functions */
    workers[0] = new_pthread(calc_avg, &calc_args);
    workers[1] = new_pthread(calc_max, &calc_args);
    workers[2] = new_pthread(calc_min, &calc_args);
    workers[3] = new_pthread(calc_med, &calc_args);

    for (i = 0; i < 4; i++)
        pthread_join(workers[i], NULL);

    printf("The average value calculated by first thread one is %.2f\n", calc_args.avg);
    printf("The maximum value calculated by second thread is %d\n", calc_args.max);
    printf("The minimum value calculated by third thread is %d\n", calc_args.min);
    printf("The median value calculated by fourth thread is %.2f\n", calc_args.med);
}

pthread_t new_pthread(void *func, void *param)
{
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (pthread_create(&tid, &attr, func, param) != 0)
    {
        perror("q2. new_pthread: pthread_create() error");
        exit(1);
    }
    return tid;
}

float calc_avg(CALC_ARGS *calc_args)
{
    int i;
    float avg = 0;
    for (i = 0; i < calc_args->n; i++)
    {
        avg += calc_args->arr[i];
    }
    avg = (int)(avg / calc_args->n); // TODO: double check why example is rounded
    calc_args->avg = avg;
    return avg;
}
int calc_max(CALC_ARGS *calc_args)
{
    int i;
    int max;
    if (calc_args->n > 0)
    {
        max = calc_args->arr[0];
        for (i = 0; i < calc_args->n; i++)
        {
            if (calc_args->arr[i] > max)
                max = calc_args->arr[i];
        }
        calc_args->max = max;
        return max;
    }
    else
        printf("q2. calc_max: Error, No values in array.");
    return -1;
}
int calc_min(CALC_ARGS *calc_args)
{
    int i;
    int min;
    if (calc_args->n > 0)
    {
        min = calc_args->arr[0];
        for (i = 0; i < calc_args->n; i++)
        {
            if (calc_args->arr[i] < min)
                min = calc_args->arr[i];
        }
        calc_args->min = min;
        return min;
    }
    else
        printf("q2. calc_min: Error, No values in array.");
    return -1;
}
float calc_med(CALC_ARGS *calc_args)
{
    int count = 0, i, j, slot;
    int sorted_arr[calc_args->n];
    float median;
    if (calc_args->n > 0)
    {
        for (i = 0; i < calc_args->n; i++)
        {
            slot = count;
            /* find correct slot to create sorted array */
            for (j = count - 1; j >= 0; j--)
            {
                if (sorted_arr[j] > calc_args->arr[i])
                    slot = j;
            }
            for (j = calc_args->n - 1; j > slot; j--)
                sorted_arr[j] = sorted_arr[j - 1];
            sorted_arr[slot] = calc_args->arr[i];
            count++;
        }
        if (calc_args->n % 2 == 0)
            median = (sorted_arr[(int)(calc_args->n / 2)] + sorted_arr[(int)(calc_args->n / 2) - 1]) / 2.0;
        else
            median = sorted_arr[(int)(calc_args->n / 2)];
        calc_args->med = median;
        return median;
    }
    else
        printf("q2. calc_med: Error, No values in array.");
    return -1;
}