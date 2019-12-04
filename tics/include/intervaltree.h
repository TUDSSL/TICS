/* Based on:
 *  https://www.geeksforgeeks.org/interval-tree/
 *  http://www.davismol.net/2016/02/07/data-structures-augmented-interval-tree-to-search-for-interval-overlapping/
 */
#ifndef INTERVALTREE_H__
#define INTERVALTREE_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Only finds the first interval
 * The VM should not have overlapping cached sections, so thats fine.
 * If all results must be found the return should be replaced with something
 * to add results to a list and just continue.
 */

struct interval
{
    INTERVAL_TYPE low, high;
};

typedef struct interval_node
{
    uint16_t valid_node;
    struct interval_node *left, *right;
    struct interval interval;
    INTERVAL_TYPE max;
    char data[];
} interval_node_t;

typedef void (*intervaltree_callback)(char *data, INTERVAL_TYPE, INTERVAL_TYPE, INTERVAL_TYPE, uint16_t);

/* Prototypes */
static interval_node_t * intervaltree_new_node(INTERVAL_TYPE low, INTERVAL_TYPE high);
static interval_node_t *intervaltree_insert(interval_node_t **new_node, interval_node_t *root, INTERVAL_TYPE low, INTERVAL_TYPE high);
static bool intervaltree_overlap(INTERVAL_TYPE l1, INTERVAL_TYPE h1, INTERVAL_TYPE l2, INTERVAL_TYPE h2);
static interval_node_t *intervaltree_overlap_search(interval_node_t *root, INTERVAL_TYPE low, INTERVAL_TYPE high);

static interval_node_t * intervaltree_new_node(INTERVAL_TYPE low, INTERVAL_TYPE high)
{
    interval_node_t *inode;

    //inode = &IntervalPool[IntervalPoolIdx];
    //IntervalPoolIdx++;
    inode = INTERVAL_ALLOC(high-low+1 + sizeof(interval_node_t));
    if (inode == NULL) {
        return NULL;
    }

    inode->interval.low = low;
    inode->interval.high = high;
    inode->max = high;
    inode->left = NULL;
    inode->right = NULL;
    inode->valid_node = 0;

    return inode;
}

static interval_node_t *intervaltree_insert(interval_node_t **new_node, interval_node_t *node, INTERVAL_TYPE low, INTERVAL_TYPE high)
{
    // Tree is empty, new node becomes node
    if (node == NULL) {
        *new_node = intervaltree_new_node(low, high);
        return *new_node;
    }

    // Get low value of intercal at node
    INTERVAL_TYPE l = node->interval.low;

    // If node's low value is smaller, then new interval goes to left subtree
    if (low < l) {
        node->left = intervaltree_insert(new_node, node->left, low, high);
    } else {
        node->right = intervaltree_insert(new_node, node->right, low, high);
    }

    // Update the max value of this ancestor if needed
    if (node->max < high) {
        node->max = high;
    }

    return node;
}

static bool intervaltree_overlap(INTERVAL_TYPE l1, INTERVAL_TYPE h1, INTERVAL_TYPE l2, INTERVAL_TYPE h2)
{
    if ((l1 <= h2) &&(l2 <= h1)) {
        return true;
    }
    return false;
}

static interval_node_t *intervaltree_overlap_search(interval_node_t *root, INTERVAL_TYPE low, INTERVAL_TYPE high)
{
    if (root == NULL) {
        return NULL;
    }

    // If interval overlaps with root
    if (intervaltree_overlap(root->interval.low, root->interval.high, low, high)) {
        return root;
    }

    // If left child of root is present and max of left child is greater than
    // or equal to the given interval, then the search interval may overlap
    // with an interval in its left subtree
    if ((root->left != NULL) && (root->left->max >= low)) {
        return intervaltree_overlap_search(root->left, low, high);
    }

    // Otherwise the search interval can only overlap with the right subtree
    return intervaltree_overlap_search(root->right, low, high);
}

static void intervaltree_inorder(interval_node_t *root, intervaltree_callback cb)
{
    if (root == NULL) {
        return;
    }
    intervaltree_inorder(root->left, cb);

    cb(root->data, root->interval.low, root->interval.high, root->max, root->valid_node);
    intervaltree_inorder(root->right, cb);
}

#endif /* INTERVALTREE_H__ */
