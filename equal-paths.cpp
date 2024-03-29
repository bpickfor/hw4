#ifndef RECCHECK
// if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here

// plan:
// base case: tree empty, return true (empty IS all paths equal by def)
// init leaf height to -1 (bc you havent gotten to any yet)
// call helper function with root node (height of 0)

// in helper function
/*
check if current node is a leaf node (no kids eexist)
   if true:
      set height for first leaf encountered
      check if curr leaf height is equal to first leaf height
   if not leaf (left child exists)
      recursively check left subtree, remem to increase height but not leaf height
    if not leaf (right child exists)
       recursively chekc right subtree, same as L
    return true if left andf right subtrees are true, else false
*/

// proto for helper funt:
bool helperEqualPaths(Node *node, int currH, int &leafH);

bool equalPaths(Node *root)
{
    // Add your code below
    if (!root)
    {
        return true;
    }
    int leafH = -1;
    return helperEqualPaths(root, 0, leafH);
}

// helper funct
bool helperEqualPaths(Node *node, int currH, int &leafH)
{
    if (!node->left && !node->right)
    { // if left sub DNE and right sub DNE (aka found a leaf)
        if (leafH == -1)
        {                  // if no leaves encountered yet
            leafH = currH; // make leaf height now the height of the leaf u just found
            return true;
        }
        return (currH == leafH); // if same height return true
    }

    // init these to true
    bool L_eq = true;
    bool R_eq = true;

    if (node->left)
    {
        L_eq = helperEqualPaths(node->left, currH + 1, leafH); // recursively check left subtree
        // go to next left node, increase height by 1, and pass in leaf height (shouldnnt change after 1 leaf found, else -1)
    }
    if (node->right)
    {
        R_eq = helperEqualPaths(node->right, currH + 1, leafH);
        // same for right subtree
    }
    return (L_eq && R_eq);
}