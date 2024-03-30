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

    // rotation stuff
    AVLNode<Key, Value> *rotateLeft(AVLNode<Key, Value> *node);
    AVLNode<Key, Value> *rotateRight(AVLNode<Key, Value> *node);
    // AVLNode<Key, Value> *rotateLeftRight(AVLNode<Key, Value> *node);
    // AVLNode<Key, Value> *rotateRightLeft(AVLNode<Key, Value> *node);

    // insert helper
    AVLNode<Key, Value> *insertNode(AVLNode<Key, Value> *node, const std::pair<const Key, Value> &new_item);
    // remove helper
    AVLNode<Key, Value> *removeNode(AVLNode<Key, Value> *node, const Key &key);

    // update height and blaance
    void updateHeightAndBalance(AVLNode<Key, Value> *node);
    // get height
    int height(AVLNode<Key, Value> *node) const;
    // rebalance at a given node
    AVLNode<Key, Value> *rebalance(AVLNode<Key, Value> *node);
    // get balance of node
    int getBalance(AVLNode<Key, Value> *node) const;
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO
    this->root_ = insertNode(dynamic_cast<AVLNode<Key, Value> *>(this->root_), new_item);
}

// insert helper
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::insertNode(AVLNode<Key, Value> *node, const std::pair<const Key, Value> &new_item)
{
    if (node == nullptr)
    {
        return new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    }

    if (new_item.first < node->getKey())
    {
        node->setLeft(insertNode(node->getLeft(), new_item));
        node->getLeft()->setParent(node);
    }
    else if (new_item.first > node->getKey())
    {
        node->setRight(insertNode(node->getRight(), new_item));
        node->getRight()->setParent(node);
    }
    else
    {
        node->setValue(new_item.second);
        return node;
    }

    updateHeightAndBalance(node);
    return rebalance(node);
}

// rebalance helper
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::rebalance(AVLNode<Key, Value> *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    updateHeightAndBalance(node);

    int balance = node->getBalance();

    if (balance > 1)
    {
        if (node->getLeft() != nullptr && node->getLeft()->getBalance() < 0)
        {
            node->setLeft(rotateLeft(node->getLeft()));
        }
        return rotateRight(node);
    }
    else if (balance < -1)
    {
        if (node->getRight() != nullptr && node->getRight()->getBalance() > 0)
        {
            node->setRight(rotateRight(node->getRight()));
        }
        return rotateLeft(node);
    }

    return node;
}

// rotation stuff
// rotate left
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *pivotNode)
{
    AVLNode<Key, Value> *rightChild = pivotNode->getRight();
    pivotNode->setRight(rightChild->getLeft());

    if (rightChild->getLeft() != nullptr)
    {
        rightChild->getLeft()->setParent(pivotNode);
    }

    rightChild->setParent(pivotNode->getParent());

    if (pivotNode->getParent() == nullptr)
    {
        this->root_ = rightChild;
    }
    else if (pivotNode == pivotNode->getParent()->getLeft())
    {
        pivotNode->getParent()->setLeft(rightChild);
    }
    else
    {
        pivotNode->getParent()->setRight(rightChild);
    }

    rightChild->setLeft(pivotNode);
    pivotNode->setParent(rightChild);

    updateHeightAndBalance(pivotNode);
    updateHeightAndBalance(rightChild);

    return rightChild;
}
// rotate right
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *pivotNode)
{
    AVLNode<Key, Value> *leftChild = pivotNode->getLeft();
    pivotNode->setLeft(leftChild->getRight());

    if (leftChild->getRight() != nullptr)
    {
        leftChild->getRight()->setParent(pivotNode);
    }

    leftChild->setParent(pivotNode->getParent());

    if (pivotNode->getParent() == nullptr)
    {
        this->root_ = leftChild;
    }
    else if (pivotNode == pivotNode->getParent()->getLeft())
    {
        pivotNode->getParent()->setLeft(leftChild);
    }
    else
    {
        pivotNode->getParent()->setRight(leftChild);
    }

    leftChild->setRight(pivotNode);
    pivotNode->setParent(leftChild);

    updateHeightAndBalance(pivotNode);
    updateHeightAndBalance(leftChild);

    return leftChild;
}

// height blaance helper
template <class Key, class Value>
void AVLTree<Key, Value>::updateHeightAndBalance(AVLNode<Key, Value> *node)
{
    if (node != nullptr)
    {
        int leftHeight = height(node->getLeft());
        int rightHeight = height(node->getRight());
        node->setBalance(leftHeight - rightHeight);
    }
}

// get height helper
template <class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value> *node) const
{
    if (node == nullptr)
    {
        return -1;
    }
    return 1 + std::max(height(node->getLeft()), height(node->getRight()));
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key)
{
    // TODO
    this->root_ = removeNode(dynamic_cast<AVLNode<Key, Value> *>(this->root_), key);
}

// helper for remove
template <class Key, class Value>
AVLNode<Key, Value> *AVLTree<Key, Value>::removeNode(AVLNode<Key, Value> *node, const Key &key)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    // normal bst remove
    if (key < node->getKey())
    {
        node->setLeft(removeNode(node->getLeft(), key));
    }
    else if (key > node->getKey())
    {
        node->setRight(removeNode(node->getRight(), key));
    }
    else
    {
        // 1 child or no child
        if (node->getLeft() == nullptr || node->getRight() == nullptr)
        {
            AVLNode<Key, Value> *temp = node->getLeft() ? node->getLeft() : node->getRight();
            if (temp == nullptr)
            { // no child
                temp = node;
                node = nullptr;
            }
            else
            {                  // 1 kid
                *node = *temp; // store content
            }
            delete temp;
        }
        else
        {
            // node with 2 kids
            AVLNode<Key, Value> *temp = dynamic_cast<AVLNode<Key, Value> *>(this->getSmallestNode(node->getRight()));
            node->setKey(temp->getKey());
            node->setValue(temp->getValue());
            node->setRight(removeNode(node->getRight(), temp->getKey()));
        }
    }

    if (node == nullptr)
    {
        return node;
    }

    updateHeightAndBalance(node);
    return rebalance(node);
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
