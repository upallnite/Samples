#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "maze.h"
#include "d_heap.h"

// Computes the optimal setting of d for d_heap
int find_d()
{
    int e, v;

    v = height * width; // aprox. number of vertices
    #ifdef EIGHTCONNECTED
    e = 8*v - 6*(height + width) + 4; // approx. number of edges
    #else
    e = 4*v - 2*(height + width);
    #endif
    return (int) round(e / v);
}

// Initialize new d-heap object
d_heap* d_heap_new()
{
    d_heap* h;
    h = calloc(1, sizeof(d_heap));
    h->size = 0;
    h->d = find_d();

    // make priority queue by inserting cells into d-heap...
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            d_heap_insert(h, &maze[j][i], LARGE);
        }
    }
    decrease_key(h, goal->d_heap_index, 0);

    return h;
}

// Initialize new d-heap object for fixing cycles
d_heap* d_heap_cycle_fix(cell *target, int top, int bottom, int left, int right)
{
    int e, v, x_len, y_len;
    d_heap* h;
    cell *temp;

    h = calloc(1, sizeof(d_heap));
    h->size = 0;
    
    x_len = right - left;
    y_len = bottom - top;

    // find d
    v = x_len * y_len;
   #ifdef EIGHTCONNECTED
    e = 8*v - 6*(x_len + y_len) + 4; // approx. number of edges
    #else
    e = 4*v - 2*(x_len + y_len);
    #endif

    h->d = (int) round(e / v);

    // make priority queue by inserting cells into d-heap...
    for (int j = top; j <= bottom; j++) {
        for (int i = left; i <= right; i++) {
            temp = get_cell(i,j);
            if (!temp->obstacle)
                d_heap_insert(h, temp, LARGE);
            else
                temp->known_obstacle = 1;
        }
    }
    decrease_key(h, target->d_heap_index, 0);

    return h;
}

// Insert cell into d-heap
void d_heap_insert(d_heap *h, cell *c, long int key)
{
    h->array[h->size++] = c;
    c->d_heap_index = h->size - 1;
    c->d_heap_key = LARGE;
    decrease_key(h, h->size - 1, key);
}

// Decrease cell key in array[index]
void decrease_key(d_heap *h, long int index, long int key)
{
    cell *c = h->array[index];
    long int parent = ((index-1)/h->d);

    if (key > c->d_heap_key) {
        printf("ERROR: Given key is greater than inital value.\n");
        exit(EXIT_FAILURE);
    }

    c->d_heap_key = key;

    // While index is less than root AND it's parent is greater...
    while ((index > 0) && (h->array[parent]->d_heap_key > c->d_heap_key)) 
    {
        // swap index and it's parent.
        h->array[index] = h->array[parent];
        h->array[index]->d_heap_index = index; 

        h->array[parent] = c;
        c->d_heap_index = parent; 

        index = parent;
        parent = ((index-1)/h->d);
    }
}

// Return cell with min key
cell* d_heap_min(d_heap *h)
{
    if (h->size == 0) return NULL;
    return h->array[1];
}


// Extract cell with min key
cell* d_heap_extract_min(d_heap *h)
{
    cell* c;
    
    if (h->size < 1) return NULL;

    c = h->array[0];
    h->array[0] = h->array[--h->size];
    h->array[0]->d_heap_index = 0;
    min_heapify(h, 0);

    return c;
}

// MinHeapify: maintain min-heap properties
void min_heapify (d_heap *h, long int index)
{
    cell *c;
    long int child = h->d*(index) + 1;
    long int smallest = index;

    // For each child of array[index]...
    for (long int i = child; i < child + h->d; i++)
    {
        if (i > h->size - 1) 
            break;
        if (h->array[i]->d_heap_key < h->array[smallest]->d_heap_key)
            smallest = i;
    } 

    if (smallest != index) {
        // exchange index and smallest
        c = h->array[smallest];
        h->array[smallest] = h->array[index];
        h->array[smallest]->d_heap_index = smallest;

        h->array[index] = c;
        c->d_heap_index = index;

        min_heapify(h, smallest);
    }
}

// Clear d-heap
void d_heap_empty(d_heap *h)
{
    long int i;
    for (i = 1; i <= h->size; ++i)
    {
        h->array[i] = 0;
    }
    h->size = 0;
}
 
void d_heap_free(d_heap *h)
{
    d_heap_empty(h);
    free(h);
}

// A function to help test if the d_heap struct is working correctly
void d_heap_test()
{
    cell *c = get_cell(1, 1);
    long int key;
    d_heap* h = d_heap_new(3);
    
    for (int i = 1; i < 11; i++) 
    {
        c = get_cell(1, i);
        key = rand() % 1000;
        d_heap_insert(h, c, key);
        printf("key:%ld  index:%ld\n", c->d_heap_key, c->d_heap_index);
    } 
    
    while (h->size > 0)
    {
        c = d_heap_extract_min(h);
        printf("key:%ld  index:%ld\n", c->d_heap_key, c->d_heap_index);
    }
}




