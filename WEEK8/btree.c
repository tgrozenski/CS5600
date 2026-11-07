#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAXK 3
#define MAXC 4

typedef struct node
{
    int keys[MAXK];
    struct node *child[MAXC];
    int n;
    int leaf;
} node_t;

typedef struct
{
    node_t *root;
    pthread_mutex_t lock;
} btree_t;

btree_t t;
int ops;

node_t *new_node()
{
    node_t *x = calloc(1, sizeof(node_t));
    x->leaf = 1;
    return x;
}

void split_child(node_t *x, int i)
{
    node_t *y = x->child[i];
    node_t *z = new_node();

    z->leaf = y->leaf;
    z->n = 1;
    z->keys[0] = y->keys[2];
    if (!y->leaf)
    {
        z->child[0] = y->child[2];
        z->child[1] = y->child[3];
    }

    y->n = 1;
    for (int j = x->n; j >= i + 1; j--)
    {
        x->child[j + 1] = x->child[j];
    }
    x->child[i + 1] = z;

    for (int j = x->n - 1; j >= i; j--)
    {
        x->keys[j + 1] = x->keys[j];
    }
    x->keys[i] = y->keys[1];
    x->n++;
}

void insert_nonfull(node_t *x, int k)
{
    int i = x->n - 1;

    if (x->leaf)
    {
        while (i >= 0 && k < x->keys[i])
        {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = k;
        x->n++;
    }
    else
    {
        while (i >= 0 && k < x->keys[i])
        {
            i--;
        }
        i++;
        if (x->child[i]->n == MAXK)
        {
            split_child(x, i);
            if (k > x->keys[i])
            {
                i++;
            }
        }
        insert_nonfull(x->child[i], k);
    }
}

void insert(int k)
{
    node_t *r = t.root;

    if (r->n == MAXK)
    {
        node_t *s = new_node();
        s->leaf = 0;
        s->child[0] = r;
        t.root = s;
        split_child(s, 0);
        insert_nonfull(s, k);
    }
    else
    {
        insert_nonfull(r, k);
    }
}

int search(node_t *x, int k)
{
    int i = 0;
    while (i < x->n && k > x->keys[i])
    {
        i++;
    }
    if (i < x->n && k == x->keys[i])
    {
        return 1;
    }
    if (x->leaf)
    {
        return 0;
    }
    return search(x->child[i], k);
}

void *worker(void *a)
{
    unsigned s = (unsigned)(long)a;

    for (int i = 0; i < ops; i++)
    {
        s = s * 1664525 + 1013904223;
        pthread_mutex_lock(&t.lock);
        search(t.root, (int)(s % 10000));
        pthread_mutex_unlock(&t.lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int n = atoi(argv[1]);
    ops = atoi(argv[2]);

    pthread_mutex_init(&t.lock, NULL);
    t.root = new_node();
    for (int i = 0; i < 5000; i++)
    {
        insert(i * 2);
    }

    pthread_t *th = malloc(n * sizeof(pthread_t));
    struct timeval a, b;

    gettimeofday(&a, NULL);
    for (int i = 0; i < n; i++)
    {
        pthread_create(&th[i], NULL, worker, (void *)(long)(i + 1));
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(th[i], NULL);
    }
    gettimeofday(&b, NULL);

    double secs = (b.tv_sec - a.tv_sec) + (b.tv_usec - a.tv_usec) / 1000000.0;
    printf("time: %f seconds (threads=%d)\n", secs, n);
    return 0;
}
