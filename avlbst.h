#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

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
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
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
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int8_t diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* g)
{
  AVLNode<Key,Value>* p = g->getLeft();
  AVLNode<Key,Value>* par = g->getParent();
  AVLNode<Key,Value>* c = p->getRight();

  p->setParent(par);
  if(par == NULL) {this->root_ = p;}
  else if(par->getLeft() == g) {par->setLeft(p);}
  else{par->setRight(p);}

  p->setRight(g);
  g->setParent(p);
  g->setLeft(c);
  if(c != NULL) {c->setParent(g);}


}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* g)
{
  AVLNode<Key,Value>* p = g->getRight();
  AVLNode<Key,Value>* par = g->getParent();
  AVLNode<Key,Value>* c = p->getLeft();

  p->setParent(par);
  if(par == NULL) {this->root_ = p;}
  else if(par->getLeft() == g) {par->setLeft(p);}
  else{par->setRight(p);}

  p->setLeft(g);
  g->setParent(p);
  g->setRight(c);
  if(c != NULL) {c->setParent(g);}

}


template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key,Value>* curr = static_cast<AVLNode<Key,Value>*>(this->root_);
    while (true) {
        if (new_item.first < curr->getKey()) {
            if (curr->getLeft() == NULL) {
                AVLNode<Key,Value>* n = new AVLNode<Key,Value>(new_item.first, new_item.second, curr);
                curr->setLeft(n);
                curr->updateBalance(-1);
                if (curr->getBalance() != 0) insertFix(curr, n);
                return;
            }
            curr = curr->getLeft();
        }
        else if (curr->getKey() < new_item.first) {
            if (curr->getRight() == NULL) {
                AVLNode<Key,Value>* n = new AVLNode<Key,Value>(new_item.first, new_item.second, curr);
                curr->setRight(n);
                curr->updateBalance(1);
                if (curr->getBalance() != 0) insertFix(curr, n);
                return;
            }
            curr = curr->getRight();
        }
        else {
            curr->setValue(new_item.second);   // duplicate: overwrite, no balance change
            return;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
  if (p == NULL) return;
    AVLNode<Key,Value>* g = p->getParent();
    if (g == NULL) return;

    bool pIsLeft = (g->getLeft() == p);
    g->updateBalance(pIsLeft ? -1 : 1);

    if (g->getBalance() == 0) return;
    if (g->getBalance() == -1 || g->getBalance() == 1) {
        insertFix(g, p);
        return;
    }

    // g is +2 or -2: rotate
    if (pIsLeft) {                              // g is -2
        if (p->getLeft() == n) {                // zig-zig
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        } else {                                // zig-zag
            rotateLeft(p);
            rotateRight(g);
            int8_t b = n->getBalance();
            if (b == -1)      { p->setBalance(0);  g->setBalance(1); }
            else if (b == 0)  { p->setBalance(0);  g->setBalance(0); }
            else              { p->setBalance(-1); g->setBalance(0); }
            n->setBalance(0);
        }
    } else {                                    // g is +2
        if (p->getRight() == n) {               // zig-zig
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        } else {                                // zig-zag
            rotateRight(p);
            rotateLeft(g);
            int8_t b = n->getBalance();
            if (b == 1)       { g->setBalance(-1); p->setBalance(0); }
            else if (b == 0)  { g->setBalance(0);  p->setBalance(0); }
            else              { g->setBalance(0);  p->setBalance(1); }
            n->setBalance(0);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key,Value>* n = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    if (n == NULL) return;

    if (n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key,Value>* pred =
            static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    AVLNode<Key,Value>* p = n->getParent();
    int8_t diff = 0;
    if (p != NULL) diff = (p->getLeft() == n) ? 1 : -1;

    AVLNode<Key,Value>* child = (n->getLeft() != NULL) ? n->getLeft() : n->getRight();
    if (child != NULL) child->setParent(p);

    if (p == NULL) this->root_ = child;
    else if (p->getLeft() == n) p->setLeft(child);
    else p->setRight(child);

    delete n;
    removeFix(p, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int8_t diff)
{
  if (n == NULL) return;

    AVLNode<Key,Value>* p = n->getParent();
    int8_t ndiff = 0;
    if (p != NULL) ndiff = (p->getLeft() == n) ? 1 : -1;

    int nb = n->getBalance() + diff;

    if (nb == -2) {
        AVLNode<Key,Value>* c = n->getLeft();
        int8_t cb = c->getBalance();
        if (cb == -1) {                         // zig-zig
            rotateRight(n);
            n->setBalance(0); c->setBalance(0);
            removeFix(p, ndiff);
        } else if (cb == 0) {                   // zig-zig, height unchanged
            rotateRight(n);
            n->setBalance(-1); c->setBalance(1);
            return;
        } else {                                // zig-zag
            AVLNode<Key,Value>* g = c->getRight();
            int8_t gb = g->getBalance();
            rotateLeft(c);
            rotateRight(n);
            if (gb == 1)      { n->setBalance(0); c->setBalance(-1); }
            else if (gb == 0) { n->setBalance(0); c->setBalance(0); }
            else              { n->setBalance(1); c->setBalance(0); }
            g->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (nb == 2) {
        AVLNode<Key,Value>* c = n->getRight();
        int8_t cb = c->getBalance();
        if (cb == 1) {                          // zig-zig
            rotateLeft(n);
            n->setBalance(0); c->setBalance(0);
            removeFix(p, ndiff);
        } else if (cb == 0) {                   // zig-zig, height unchanged
            rotateLeft(n);
            n->setBalance(1); c->setBalance(-1);
            return;
        } else {                                // zig-zag
            AVLNode<Key,Value>* g = c->getLeft();
            int8_t gb = g->getBalance();
            rotateRight(c);
            rotateLeft(n);
            if (gb == 1)      { n->setBalance(-1); c->setBalance(0); }
            else if (gb == 0) { n->setBalance(0);  c->setBalance(0); }
            else              { n->setBalance(0);  c->setBalance(1); }
            g->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else if (nb == 0) {
        n->setBalance(0);
        removeFix(p, ndiff);                    // subtree got shorter: keep going
    }
    else {                                      // nb is +1 or -1
        n->setBalance(nb);
        return;                                 // height unchanged: stop
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
