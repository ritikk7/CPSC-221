#include "chain.h"
#include <cmath>
#include <iostream>


// PA1 functions

/**
 * Destroys the current Chain. This function should ensure that
 * memory does not leak on destruction of a chain.
 */
Chain::~Chain() {
  clear();
}

/**
 * Inserts a new node after the node pointed to by p in the
 * chain (so p->next is the new node) and returns a pointer to
 * the newly created node.
 * If p is NULL, inserts a new head node to the chain.
 * This function **SHOULD** create a new Node and increase length_.
 *
 * @param p = The new node should be pointed to by p->next.
 *            If p is NULL, the new node becomes the head of the chain.
 * @param ndata = The data to be inserted.
 */
Chain::Node * Chain::insertAfter(Node * p, const Block &ndata) {
  Node* newNode = new Node(ndata);

  if (p == NULL) {
    head_ = newNode;
    length_++;
    return head_;
  }
  if (p->next == NULL) {
    p->next = newNode;
    newNode->prev = p;
    length_++;
    return newNode;
  } else {
    newNode->next = p->next;
    p->next->prev = newNode;
    p->next = newNode;
    newNode->prev = p;
    length_++;
    return newNode;
  }
}

/**
 * Swaps the position in the chain of the two nodes pointed to
 * by p and q.
 * If p or q is NULL or p==q, do nothing.
 * Change the chain's head pointer if necessary.
 */
void Chain::swap(Node *p, Node *q) {

  if (p == NULL || q == NULL || p == q) {
    return;
  }

  if (p == head_) {
    head_ = q;
  } else if (q == head_) {
    head_ = p;
  }

  Node *p_prev = p->prev;
  Node *p_next = p->next;
  Node *q_prev = q->prev;
  Node *q_next = q->next;

  
  if (q_prev != NULL && q_prev != p) {
    q_prev->next = p;
  }
  if (q_next != NULL && q_next != p) {
    q_next->prev = p;
  }
  if (p_prev != NULL && p_prev != q) {
    p_prev->next = q;
  }
  if (p_next != NULL && p_next != q) {
    p_next->prev = q;
  }

  if (q->next == p) {
    q->next = p_next;
    p->prev = q_prev;
    p->next = q;
    q->prev = p;
  } else if (p->next == q) {
    p->next = q_next;
    q->prev = p_prev;
    p->prev = q;
    q->next = p;
  } else {
    p->next = q_next;
    p->prev = q_prev;
    q->next = p_next;
    q->prev = p_prev;
  }
}

/**
 * Destroys all dynamically allocated memory associated with the
 * current Chain class.
 */
 void Chain::clear() {
  Node *current = head_;
  Node *next = current;
  while (current != NULL) {
    next = current->next;
    delete current;
    current = next;
  }
  length_ = 0;
}

/**
 * Makes the current object into a copy of the parameter:
 * All member variables should have the same value as
 * those of other, but the memory should be completely
 * independent. This function is used in both the copy
 * constructor and the assignment operator for Chains.
 */
void Chain::copy(Chain const &other) {
  head_ = new Node(other.head_->data);
  Node *current = head_;
  Node *currother = other.head_->next;
  while (currother != NULL) {
    current->next = new Node(currother->data);
    current = current->next;
    currother = currother->next;
  }
  length_ = other.length_;
}

/* Modifies the current chain: 
 * 1) Find the node with the first (leftmost) block in the unscrambled
 *    image and move it to the head of the chain.
 *	This block is the one whose closest match (to the left) is the
 *	largest.  That is, the distance (using distanceTo) to this block 
 *	is big for all other blocks.
 *	For each block B, find the distanceTo B from every other block
 *	and take the minimum of these differences as B's "value".
 *	Choose the block B with the maximum value over all blocks and
 *	swap its node to the head of the chain.
 *
 * 2) Starting with the (just found) first block B, find the node with
 *    the block that is the closest match to follow B (to the right)
 *    among the remaining blocks, move (swap) it to follow B's node,
 *    then repeat to unscramble the chain/image.
 */
void Chain::unscramble() {
  /* your code here */
  if (head_ == NULL || head_->next == NULL) {
    return;
  }
  int minDist = 0;
  int max = 0;
  Node *current = head_;
  while (current != NULL) {
    current = current->next;
  }
  double differences[length_];
  double data[length_];
  current = head_;
  Node *temp = head_;
  int c1 = 0;
  while (c1 < length_) {  
    int c2 = 0;
    while (c2 < length_) {
      differences[c2] = temp->data.distanceTo(current->data);
      if (differences[minDist] > differences[c2] && current != temp) {
        minDist = c2;
      }
      temp = temp->next;
      c2++;
    }
    data[c1] = differences[minDist];
    minDist = 0;
    if (data[max] < data[c1]) {
      max = c1;
    }
    current = current->next;
    temp = head_;
    c1++;
  }
  for (int i = 0; i < max; i++)
    temp = temp->next;
  swap(head_, temp);

  if (length_ == 2) {
    return;
  }

  minDist = 1;
  current = head_;
  temp = head_->next;
  Node *node = head_;
  int c3 = 0;
  while (c3 < length_-1) {
    minDist = c3 + 1;
    int c4 = c3 + 1;
    while (c4 < length_) {
      differences[c4] = current->data.distanceTo(temp->data);
        if (differences[minDist] >= differences[c4]) {
          minDist = c4;
          node = temp;
        }
      temp = temp->next;
      c4++;
    }
    swap(current->next, node);
    current = current->next;
    temp = current->next;
    c3++;
  }
}
