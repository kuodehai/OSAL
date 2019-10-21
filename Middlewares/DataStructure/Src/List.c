/**************************************************************************************************
  Filename:       List.c
  Revised:        $Date: 2014-11-04 15:36:27 -0800 (Tue, 04 Nov 2014) $
  Revision:       $Revision: 40989 $

  Description:    This implements a doubly linked list module for the stack
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"

#include "List.h"

/**
 *  @brief  Function to initialize the contents of an list_list
 *
 *  @param  list Pointer to an list_list structure that will be used to
 *               maintain a linked list
 */
void list_clearList(list_list *list)
{
  list->head = list->tail = NULL;
}

/**
 *  @brief  Function to test whether a linked list is empty
 *
 *  @param  list A pointer to a linked list
 *
 *  @return true if empty, false if not empty
 */
_bool list_empty(list_list *list)
{
  return (list->head == NULL);
}

/**
 *  @brief  Function to atomically get the first elem in a linked list
 *
 *  @param  list A pointer to a linked list
 *
 *  @return Pointer the first elem in the linked list or NULL if empty
 */
list_elem *list_get(list_list *list)
{
  list_elem *elem;

  elem = list->head;

  /* See if the List was empty */
  if(elem != NULL)
  {
    list->head = elem->next;

    if(elem->next != NULL)
    {
      elem->next->prev = NULL;
    }
    else
    {
      list->tail = NULL;
    }
  }

  return (elem);
}

/**
 *  @brief  Function to return the head of a linked list
 *
 *  This function does not remove the head, it simply returns a pointer to
 *  it. This function is typically used when traversing a linked list.
 *
 *  @param  list A pointer to the linked list
 *
 *  @return Pointer to the first elem in the linked list or NULL if empty
 */
list_elem *list_head(list_list *list)
{
  return (list->head);
}

/**
 *  @brief  Function to insert an elem into a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  newElem New elem to insert
 *
 *  @param  curElem Elem to insert the newElem in front of.
 *          This value cannot be NULL.
 */
void list_insert(list_list *list, list_elem *newElem,
                     list_elem *curElem)
{
  newElem->next = curElem;
  newElem->prev = curElem->prev;

  if(curElem->prev != NULL)
  {
    curElem->prev->next = newElem;
  }
  else
  {
    list->head = newElem;
  }

  curElem->prev = newElem;
}

/**
 *  @brief  Function to return the next elem in a linked list
 *
 *  This function does not remove the elem, it simply returns a pointer to
 *  next one. This function is typically used when traversing a linked list.
 *
 *  @param  elem Elem in the list
 *
 *  @return Pointer to the next elem in linked list or NULL if at the end
 */
list_elem *list_next(list_elem *elem)
{
  return (elem->next);
}

/**
 *  @brief  Function to return the previous elem in a linked list
 *
 *  This function does not remove the elem, it simply returns a pointer to
 *  previous one. This function is typically used when traversing a linked list.
 *
 *  @param  elem Elem in the list
 *
 *  @return Pointer to the previous elem in linked list or NULL if at the
 *  beginning
 */
list_elem *list_prev(list_elem *elem)
{
  return (elem->prev);
}

/**
 *  @brief  Function to put an elem onto the end of a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  elem Element to place onto the end of the linked list
 */
void list_put(list_list *list, list_elem *elem)
{
  elem->next = NULL;
  elem->prev = list->tail;

  if(list->tail != NULL)
  {
    list->tail->next = elem;
  }
  else
  {
    list->head = elem;
  }

  list->tail = elem;
}

/**
 *  @brief  Function to put an elem onto the head of a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  elem Element to place onto the beginning of the linked list
 */
void list_putHead(list_list *list, list_elem *elem)
{
  elem->next = list->head;
  elem->prev = NULL;

  if(list->head != NULL)
  {
    list->head->prev = elem;
  }
  else
  {
    list->tail = elem;
  }

  list->head = elem;
}

/**
 *  @brief  Function to remove an elem from a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  elem Element to be removed from a linked list
 */
void list_remove(list_list *list, list_elem *elem)
{
  /* Handle the case where the elem to remove is the last one */
  if(elem->next == NULL)
  {
    list->tail = elem->prev;
  }
  else
  {
    elem->next->prev = elem->prev;
  }

  /* Handle the case where the elem to remove is the first one */
  if(elem->prev == NULL)
  {
    list->head = elem->next;
  }
  else
  {
    elem->prev->next = elem->next;
  }
}

/**
 *  @brief  Function to return the tail of a linked list
 *
 *  This function does not remove the tail, it simply returns a pointer to
 *  it. This function is typically used when traversing a linked list.
 *
 *  @param  list A pointer to the linked list
 *
 *  @return Pointer to the last elem in the linked list or NULL if empty
 */
list_elem *list_tail(list_list *list)
{
  return (list->tail);
}

/*********************************************************************
 */
