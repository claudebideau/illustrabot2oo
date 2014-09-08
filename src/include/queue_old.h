#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
using namespace std;

typedef int ElemType;

class Queue;

class Node
{
 friend class Queue;
 friend ostream& operator<< (ostream&, Queue&);
 protected:
       Node (const ElemType&);
       ElemType elem;
       Node*    next;
};

inline Node::Node(const ElemType& el)
: elem (el)
{
 next = NULL;
}

class Queue
{
 friend ostream& operator<< (ostream&, Queue&);

 public:
   Queue();
   ~Queue();
   Queue& insert (const ElemType&);
   ElemType remove();
   bool isEmpty();

 protected:
   Node *front;
   Node *back;
};

inline Queue::Queue()
{
 front = back = NULL;
}

inline bool Queue::isEmpty()
{
 return front == NULL ? true : false;
}

#endif
