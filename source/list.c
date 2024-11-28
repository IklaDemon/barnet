#include "list.h"

/* 
 * ltoarray() copys chars to array until 0 or NULL is reached
 * parameters:
 *   - NODE* list: first node of list
 *   - char* array: destination array passed as pointer
 * return: none
 */
void
ltoarray (NODE* list, char* array)
{
  int i = 0;
  while(list != NULL && list->c != 0)
  {
    array[i] = list->c;
    list = list->next_NODE;
    i++;
  }
  array[i] = 0;
}

/*
 * -------------------------------------------------------------------------------------------------
 * these functions abstract the list access for my main program
 */
LIST* LIST_INIT()
{
  LIST* list = (LIST*)malloc(1 * sizeof(LIST));
  if (list == NULL) return NULL;
  list->ll = NULL;
  list->index = 0;
  return list;
}

void LIST_ADD_CHAR(LIST* list, int c)
{
  list->ll = lInsertN(list->ll, list->index, c);
  list->index++;
}

void LIST_DEL_CHAR(LIST* list)
{
  list->ll = lDeleteN(list->ll, list->index);
}

void LIST_INDEX_LEFT(LIST* list)
{
  if (list->index > 0)
    list->index--;
}

void LIST_INDEX_RIGHT(LIST* list)
{
  if (list->index < llen(list->ll))
    list->index++;
}

void LIST_SET_INDEX(LIST* list, int index)
{
  if (list->index <= llen(list->ll) && list->index >= 0)
    list->index = index;
}

int LIST_GET_INDEX(LIST* list)
{
  return list->index;
}

void LIST_PRINT(LIST* list)
{
  printl (list->ll);
}

void LIST_PRINT_FROM_INDEX(LIST* list)
{
  NODE* iterator = list->ll;
  int cnt = 0;
  while (iterator != NULL)
  {
    if (cnt >= list->index)
    {
	    putchar (iterator->c);
	  }
    cnt++;
    iterator = iterator->next_NODE;
  }
}

void LIST_DELETE(LIST* list)
{
  if (list->ll != NULL) lDeleteAll(list->ll);
  list->ll = NULL;
  list->index = 0;
}

void LIST_TO_ARRAY(LIST* list, char* array)
{
  ltoarray(list->ll, array);
}

int LIST_LEN(LIST* list)
{
  return llen(list->ll);
}
/*
 * end of list asbtraction
 * -------------------------------------------------------------------------------------------------
 */

/* 
 * creates NODE
 * parameters:
 * - int  val: character
 * return:
 *  - on success: pointer to node struct
 *  - on fail: NULL pointer
 */
NODE* createn(int val)
{
  NODE* n;
  n = (NODE*)malloc(sizeof(NODE));
  if(n == NULL) return NULL;
  n->c = val;
  n->next_NODE = NULL;
  return n;
}

/*
 * returns total number of nodes in list
 * parameters:
 *  - NODE* l: first node of list
 * return:
 *  - int: number of nodes on list
 */
int llen(NODE* list)
{
  int cnt = 0;
  if (list == NULL) return cnt;
  while (list != NULL)
  {
    cnt++;
    list = list->next_NODE;
  }
  return cnt;
  /*if(l == NULL) return 0;
  if(l->next_NODE == NULL) return 1;
  else return 1 + llen(l->next_NODE);*/
}

/* 
 * Prints list characters until the first 0 char is reached (included).
 * Used for debug purposes.
 * parameters:
 *   - NODE* n: first node of list
 * return: none
 */
void printl(NODE* n)
{
  while(n != NULL)
  {
    putchar (n->c);
    n = n->next_NODE;
  }
}

/* 
 * Prints interger value of every char in list until NULL is reached.
 * Used for debug purposes.
 * parameters:
 *   - NODE* n: first node of list
 * return: none
 */
void debugl(NODE* n)
{
  do
  {
    printf ("%d ", n->c);
    n = n->next_NODE;
  } while(n != NULL);
}

/* 
 * write character in list on a specified position (index)
 * parameters:
 *   - NODE* n: first node of list
 *   - int index: position to overwrite with val
 *   - int val: character to overwrite in position index
 * return:
 *   - on success: 0
 *   - on index out of range: -1
 */
int lwrite(NODE* n, int index, int val)
{
  int i;
  if(index >= llen(n)) return -1;
  i = 0;
  while(n != NULL && i < index)
  {
    n = n->next_NODE;
    i++;
  } 
  n->c = val;
  return 0;
}

/*
 * insert character in list on a specified position (index)
 * parameters:
 *   - NODE* list: first node of list
 *   - int val: character to add
 * return:
 *   - NODE*: list
 */
NODE* lappend(NODE* list, int val)
{
  if (list == NULL) return createn(val);
  NODE* new_node;
  NODE* last_node;
  new_node = createn(val);
  last_node = lgetn(list, llen(list) - 1);
  last_node->next_NODE = new_node;
  return list;
}

/*
 * insert character in list on a specified position (index)
 * parameters:
 *   - NODE* list: first node of list
 *   - int x: position to add new node with val
 *   - int val: character to add
 * return:
 *   - NODE*: list
 *
 * usage of this function is as follows: (NODE*)list = lInsertN((NODE*)list, (int)position);
 *                                                 ^-(the updated list is returned)
 */
NODE* lInsertN(NODE* list, int x, int val)
{
  int list_length;
  NODE* new_node;
  NODE* n_x;
  NODE* n_x_1;

  // check list is not empty, otherwise creates new list with passed value
  if(list == NULL) return lappend(list, val);

  list_length = llen(list);

  // checks if x is out of range
  if(x > list_length || x < 0) return list;

  if (x == 0)
  {
    new_node = createn(0);
    if (new_node == NULL) return list;
    new_node->c = val;
    n_x = lgetn(list, x);
    new_node->next_NODE = n_x;
    return new_node;
  }
  else
  {
    if (x == list_length)
    {
      lappend(list, val);
      return list;
    }
    else
    {
      new_node = createn(0);
      if (new_node == NULL) return list;
      new_node->c = val;
      n_x = lgetn(list, x);
      n_x_1 = lgetn(list, x - 1);
      n_x_1->next_NODE = new_node;
      new_node->next_NODE = n_x;
      return list;
    }
  }
}

/* deletes node in (param)index position:
 * parameters:
 *   - NODE* list: first node of list
 *   - int index: position to add new node with val
 * return:
 *   - NODE*: list
 *
 * usage of this function is as follows: (NODE*)list = lDeletN((NODE*)list, (int)position);
 *                                                 ^-(the updated list is returned)
 */
NODE* lDeleteN(NODE* list, int index)
{
  int list_length;
  NODE* node;
  NODE* node_1;
  list_length = llen(list);
  if(index >= list_length || index < 0) return list;
  if (index == 0)
  {
    node = list->next_NODE;
    free(list);
    return node;
  }
  else
  {
    if (index == list_length - 1)
    {
      node = lgetn (list, list_length - 2);
      free(node->next_NODE);
      node->next_NODE = NULL;
      return list;
    }
    else
    {
      node = lgetn (list, index);
      node_1 = lgetn (list, index - 1);
      node_1->next_NODE = node->next_NODE;
      free(node);
      return list;
    }
  }
}

/* get node based of passed index
 * if index out of range returns NULL
 * parameters:
 *   - NODE* list: first node of list
 *   - int index: position
 * return:
 *   - NODE*: list
 */
NODE* lgetn(NODE* list, int index)
{
  int i;
  if(index >= llen(list)) return NULL;
  i = 0;
  while(i < index)
  {
    i++;
    list = list->next_NODE;
  }
  return list;
}

/*
 * sets to 0 all characters in list
 * parameters:
 *   - NODE* node: first node of list
 * return: none
 */
void leraseall(NODE* list)
{
  if(list->next_NODE == NULL) list->c = 0;
  else
  {
    leraseall(list->next_NODE);
    list->c = 0;
  }
}

/*
 * deallocates all nodes of the list
 * parameters:
 *   - NODE* node: first node of list
 * return: none
 */
void lDeleteAll(NODE* node)
{
  if (node->next_NODE == NULL)
  { 
    free(node);
    return;
  }
  else
  {
    lDeleteAll(node->next_NODE);
    free(node);
    return;
  }
}

