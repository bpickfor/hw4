#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError
{
};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value> *getParent() const override;
    virtual AVLNode<Key, Value> *getLeft() const override;
    virtual AVLNode<Key, Value> *getRight() const override;

protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class constructor
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent) : Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is necessary to make sure
 * that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value> *>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key &key);                              // TODO
protected:
    virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n);                                                 // insert helper
    void removeFix(AVLNode<Key, Value> *n, int diff);                                                               // remove helper
    void rebalance(AVLNode<Key, Value> *node);                                                                      // rebalance
    void rotateLeft(AVLNode<Key, Value> *node);                                                                     // rotate left
    void rotateRight(AVLNode<Key, Value> *node);                                                                    // rotate right
    void updateBalancesAfterDoubleRotation(AVLNode<Key, Value> *n, AVLNode<Key, Value> *c, AVLNode<Key, Value> *g); // fix balance after double rot
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO

    // normal bst insert
    AVLNode<Key, Value> *parent = nullptr;
    AVLNode<Key, Value> *current = static_cast<AVLNode<Key, Value> *>(this->root_);

    // get correct parent for new node
    while (current != nullptr)
    {
        parent = current;
        if (new_item.first < current->getKey())
        {
            current = current->getLeft();
        }
        else if (new_item.first > current->getKey())
        {
            current = current->getRight();
        }
        else
        { // duplicate key and fix val if alr exist
            current->setValue(new_item.second);
            return;
        }
    }

    // make new node
    AVLNode<Key, Value> *newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    // make new node child of parent
    if (parent == nullptr)
    {
        this->root_ = newNode; // if tree was empty new node is root (no parent)
    }
    else if (new_item.first < parent->getKey())
    {
        parent->setLeft(newNode);
    }
    else
    {
        parent->setRight(newNode);
    }

    // update balance of tree
    // start with parent of new node
    for (AVLNode<Key, Value> *node = parent; node != nullptr; node = node->getParent())
    {
        // update balance
        if (node->getLeft() == newNode)
        {
            node->updateBalance(-1);
        }
        else
        {
            node->updateBalance(1);
        }

        if (node->getBalance() == 0)
        {
            break; // height same, finish
        }
        else if (node->getBalance() < -1 || node->getBalance() > 1)
        {
            // Tree needs rebalancing
            rebalance(node);
            break; // tree balanced now, end
        }

        newNode = node; // move to parent
    }
}

// helper
template <class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *p, AVLNode<Key, Value> *n)
{
    // Precondition: p and n are balanced {-1,0,+1}
    if (p == nullptr || p->getParent() == nullptr)
        return; // if p is null or p's parent is null return

    AVLNode<Key, Value> *g = p->getParent(); // g is parent of p

    if (p == g->getLeft())
    {                         // assume p is left child of g
        g->updateBalance(-1); // update balance of g
        if (g->getBalance() == 0)
            return; // case 1
        if (g->getBalance() == -1)
        {
            insertFix(g, p); // case 2: recurse
            return;
        }
        if (g->getBalance() == -2)
        { // case 3: g is now unbalanced
            // check whether it's zig-zig or zig-zag
            if (p->getBalance() == -1)
            {
                // zig-zig case
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else
            {
                // zig-zag case
                AVLNode<Key, Value> *c = p->getRight();
                rotateLeft(p);
                rotateRight(g);
                // update balances based on c's balance
                if (c->getBalance() == -1)
                {
                    g->setBalance(1);
                }
                else
                {
                    g->setBalance(0);
                }
                if (c->getBalance() == 1)
                {
                    p->setBalance(-1);
                }
                else
                {
                    p->setBalance(0);
                }
                c->setBalance(0);
            }
        }
    }
    else
    {
        g->updateBalance(1); // Update balance of g
        if (g->getBalance() == 0)
            return; // case 1: height of subtree remains unchanged
        if (g->getBalance() == 1)
        {
            insertFix(g, p); // case 2: continue up the tree
            return;
        }
        if (g->getBalance() == 2)
        { // Case 3: g is now unbalanced
            // check whether it's zig-zig or zig-zag
            if (p->getBalance() == 1)
            {
                // zig-zig case
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else
            {
                // zig-zag case
                AVLNode<Key, Value> *c = p->getLeft();
                rotateRight(p);
                rotateLeft(g);
                // update balances based on c's balance
                if (c->getBalance() == 1)
                {
                    g->setBalance(-1);
                }
                else
                {
                    g->setBalance(0);
                }
                if (c->getBalance() == -1)
                {
                    p->setBalance(1);
                }
                else
                {
                    p->setBalance(0);
                }
                c->setBalance(0);
            }
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value> *node)
{
    // Check the balance of the node
    if (node->getBalance() < -1 || node->getBalance() > 1)
    {
        // Determine which child is taller
        AVLNode<Key, Value> *tallerChild = (node->getBalance() > 0) ? node->getRight() : node->getLeft();

        if (tallerChild != nullptr)
        {
            // Perform rotations
            if (node->getBalance() > 0)
            {
                // Check if double rotation is needed (Right-Left case)
                if (tallerChild->getBalance() < 0)
                {
                    rotateRight(tallerChild);
                }
                // Perform left rotation
                rotateLeft(node);
            }
            else
            {
                // Check if double rotation is needed (Left-Right case)
                if (tallerChild->getBalance() > 0)
                {
                    rotateLeft(tallerChild);
                }
                // Perform right rotation
                rotateRight(node);
            }
        }

        // Update balance factors
        if (node->getBalance() == -2 || node->getBalance() == 2)
        {
            node->setBalance(0);
            if (tallerChild != nullptr)
            {
                tallerChild->setBalance(0);
                AVLNode<Key, Value> *newChild = (node->getBalance() > 0) ? node->getRight() : node->getLeft();
                if (newChild != nullptr)
                {
                    newChild->setBalance(0);
                }
            }
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key)
{
    // TODO
    AVLNode<Key, Value> *n = static_cast<AVLNode<Key, Value> *>(internalFind(key));
    if (n == nullptr)
    {
        // not found
        return;
    }

    // 2 children
    if (n->getLeft() != nullptr && n->getRight() != nullptr)
    {
        AVLNode<Key, Value> *pred = static_cast<AVLNode<Key, Value> *>(predecessor(n));
        nodeSwap(n, pred);
        n = pred; // max 1 child now
    }

    AVLNode<Key, Value> *p = n->getParent();
    AVLNode<Key, Value> *child = (n->getLeft() != nullptr) ? n->getLeft() : n->getRight();

    // replace n with child
    if (p == nullptr)
    {
        this->root_ = child; // n is root
    }
    else if (n == p->getLeft())
    {
        p->setLeft(child);
    }
    else
    {
        p->setRight(child);
    }

    if (child != nullptr)
    {
        child->setParent(p);
    }

    delete n; // delete node

    // balance tree from parent of deleted node
    if (p != nullptr)
    {
        int diff = (child == p->getRight()) ? -1 : 1;
        removeFix(p, diff);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *n, int diff)
{
    // if reach root stop
    if (n == nullptr)
        return;

    // compute the next recursive call's arguments
    AVLNode<Key, Value> *p = n->getParent();
    int ndiff = (p != nullptr && n == p->getLeft()) ? 1 : -1;

    // update balance factor of n
    n->updateBalance(diff);

    // Case 1: If the balance factor becomes -2 or 2, rotations may be necessary
    if (n->getBalance() == -2 || n->getBalance() == 2)
    {
        AVLNode<Key, Value> *c = (n->getBalance() == -2) ? n->getLeft() : n->getRight();
        if (n->getBalance() == -2)
        {
            // Left cases
            if (c->getBalance() == -1)
            {
                // Left-Left case
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
            }
            else if (c->getBalance() == 0)
            {
                // Left-Left case with c balanced
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
                return; // tree height same stop here
            }
            else if (c->getBalance() == 1)
            {
                // Left-Right case
                AVLNode<Key, Value> *g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                updateBalancesAfterDoubleRotation(n, c, g);
            }
        }
        else
        {
            // Right cases
            if (c->getBalance() == 1)
            {
                // Right-Right case
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
            }
            else if (c->getBalance() == 0)
            {
                // Right-Right case with c balanced
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
                return; // tree height same stop here
            }
            else if (c->getBalance() == -1)
            {
                // eight-Left case
                AVLNode<Key, Value> *g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                updateBalancesAfterDoubleRotation(n, c, g);
            }
        }
        removeFix(p, ndiff); // Continue fixing up the tree
    }
    else if (n->getBalance() == -1 || n->getBalance() == 1)
    {
        return; // tree is balanced at n  but check ancestors
    }
    else if (n->getBalance() == 0)
    {
        removeFix(p, ndiff); // move up tree as height could have changed
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::updateBalancesAfterDoubleRotation(AVLNode<Key, Value> *n, AVLNode<Key, Value> *c, AVLNode<Key, Value> *g)
{
    if (g->getBalance() == -1)
    {
        n->setBalance(0);
        c->setBalance(1);
    }
    else if (g->getBalance() == 1)
    {
        n->setBalance(-1);
        c->setBalance(0);
    }
    else
    {
        n->setBalance(0);
        c->setBalance(0);
    }
    g->setBalance(0);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *node)
{
    AVLNode<Key, Value> *rightChild = node->getRight(); // The right child of the node

    // rightChild's left subtree is node's right subtree
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr)
    {
        rightChild->getLeft()->setParent(node);
    }

    // fix parents
    rightChild->setParent(node->getParent());
    if (node->getParent() == nullptr)
    { // node is root
        this->root_ = rightChild;
    }
    else if (node == node->getParent()->getLeft())
    { // node is a left child
        node->getParent()->setLeft(rightChild);
    }
    else
    { // node is a right child
        node->getParent()->setRight(rightChild);
    }

    // node is left child of rightChild
    rightChild->setLeft(node);
    node->setParent(rightChild);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *node)
{
    AVLNode<Key, Value> *leftChild = node->getLeft(); // The left child of the node

    // leftChild's right subtree is node's left subtree
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr)
    {
        leftChild->getRight()->setParent(node);
    }

    // fic parents
    leftChild->setParent(node->getParent());
    if (node->getParent() == nullptr)
    { // node is root
        this->root_ = leftChild;
    }
    else if (node == node->getParent()->getRight())
    { // node is a right child
        node->getParent()->setRight(leftChild);
    }
    else
    { // node is a left child
        node->getParent()->setLeft(leftChild);
    }

    //  node is right child of leftChild
    leftChild->setRight(node);
    node->setParent(leftChild);
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif