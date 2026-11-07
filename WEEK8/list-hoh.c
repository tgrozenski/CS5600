#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct node
{
    int key;
    struct node *next;
    pthread_mutex_t lock;
} node_t;

typedef struct
{
    node_t *head;
} list_t;

list_t list;
int ops;

void init()
{
    list.head = malloc(sizeof(node_t));
    list.head->key = -1;
    list.head->next = NULL;
    pthread_mutex_init(&list.head->lock, NULL);
}

int insert(int key)
{
    node_t *prev = list.head;
    node_t *curr;

    pthread_mutex_lock(&prev->lock);
    curr = prev->next;
    if (curr)
    {
        pthread_mutex_lock(&curr->lock);
    }

    while (curr)
    {
        if (curr->key >= key)
        {
            break;
        }
        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr)
        {
            pthread_mutex_lock(&curr->lock);
        }
    }

    node_t *n = malloc(sizeof(node_t));
    n->key = key;
    n->next = curr;
    pthread_mutex_init(&n->lock, NULL);

    prev->next = n;
    if (curr)
    {
        pthread_mutex_unlock(&curr->lock);
    }
    pthread_mutex_unlock(&prev->lock);
    return 0;
}

int lookup(int key)
{
    node_t *prev = list.head;
    node_t *curr;

    pthread_mutex_lock(&prev->lock);
    curr = prev->next;
    if (curr)
    {
        pthread_mutex_lock(&curr->lock);
    }

    while (curr)
    {
        if (curr->key >= key)
        {
            break;
        }
        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr)
        {
            pthread_mutex_lock(&curr->lock);
        }
    }

    int found = curr && curr->key == key;
    if (curr)
    {
        pthread_mutex_unlock(&curr->lock);
    }
    pthread_mutex_unlock(&prev->lock);
    return found;
}

int remove_key(int key)
{
    node_t *prev = list.head;
    node_t *curr;

    pthread_mutex_lock(&prev->lock);
    curr = prev->next;
    if (curr)
    {
        pthread_mutex_lock(&curr->lock);
    }

    while (curr)
    {
        if (curr->key >= key)
        {
            break;
        }
        pthread_mutex_unlock(&prev->lock);
        prev = curr;
        curr = curr->next;
        if (curr)
        {
            pthread_mutex_lock(&curr->lock);
        }
    }

    int removed = 0;
    if (curr && curr->key == key)
    {
        prev->next = curr->next;
        removed = 1;
    }
    if (curr)
    {
        pthread_mutex_unlock(&curr->lock);
    }
    pthread_mutex_unlock(&prev->lock);
    if (removed)
    {
        free(curr);
    }
    return removed;
}

void *worker(void *a)
{
    unsigned s = (unsigned)(long)a;

    for (int i = 0; i < ops; i++)
    {
        s = s * 1664525 + 1013904223;
        lookup((int)(s % 500));
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int n = atoi(argv[1]);
    ops = atoi(argv[2]);

    init();
    for (int i = 0; i < 500; i++)
    {
        insert(i);
    }

    pthread_t *t = malloc(n * sizeof(pthread_t));
    struct timeval a, b;

    gettimeofday(&a, NULL);
    for (int i = 0; i < n; i++)
    {
        pthread_create(&t[i], NULL, worker, (void *)(long)(i + 1));
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(t[i], NULL);
    }
    gettimeofday(&b, NULL);

    double secs = (b.tv_sec - a.tv_sec) + (b.tv_usec - a.tv_usec) / 1000000.0;
    printf("time: %f seconds (threads=%d)\n", secs, n);
    return 0;
}
