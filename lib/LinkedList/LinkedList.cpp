#include <Arduino.h>
#include <App.hpp>
#include "LinkedList.hpp"

void SimpleLinkedList::put( String value )
{
  if ( rootNode == NULL )
  {
    rootNode = new ListNode();
    rootNode->value = new String(value);
  }
  else
  {
    ListNode* currentListNode = NULL;

    do
    {
      if ( currentListNode == NULL ) 
      {
        currentListNode = rootNode;
      }
      else
      {
        currentListNode = currentListNode->next;
      }

      if ( *currentListNode->value == value )
        return; // make list unique
    }
    while(currentListNode->next != NULL );
    currentListNode->next = new ListNode();
    currentListNode->next->value = new String(value);
  }
}

ListNode *SimpleLinkedList::getRootNode()
{
  return rootNode;
}
