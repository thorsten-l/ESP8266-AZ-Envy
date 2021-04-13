#ifndef __LINKED_LIST_HPP__
#define __LINKED_LIST_HPP__

#include <Arduino.h>

struct ListNode
{
  String* value;
  ListNode* next = NULL;
};

class SimpleLinkedList
{

private:
  ListNode *rootNode = NULL;

public:
  void put( String value );
  ListNode* getRootNode();
};

#endif
