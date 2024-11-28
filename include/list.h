#ifndef _LIST_H_
#define _LIST_H_

#include "lib.h"

typedef struct NODE NODE;
struct NODE
{
  int c;
  NODE* next_NODE;
};

typedef struct
{
  NODE* ll;
  int index;
} LIST;

/*
 * -----------------------------------------------------------------------------
 * these functions abstract list access
 */
/*
 *
 */
LIST* LIST_INIT();
/*
 *
 */
void LIST_ADD_CHAR(LIST* list, int c);
/*
 *
 */
void LIST_DEL_CHAR(LIST* list);
/*
 *
 */
void LIST_INDEX_LEFT(LIST* list);
/*
 *
 */
void LIST_INDEX_RIGHT(LIST* list);
/*
 *
 */
void LIST_SET_INDEX(LIST* list, int index);
/*
 *
 */
int LIST_GET_INDEX(LIST* list);
/*
 *
 */
void LIST_PRINT(LIST* list);
/*
 *
 */
void LIST_PRINT_FROM_INDEX(LIST* list);
/*
 *
 */
void LIST_DELETE(LIST* list);
/*
 * LIST_TO_ARRAY () copys chars to array until 0 or NULL is reached
 */
void LIST_TO_ARRAY(LIST* list, char* array);
/*
 *
 */
int LIST_LEN(LIST* list);
/*
 * end of list asbtraction
 * -----------------------------------------------------------------------------
 */

// created node
NODE* createn(int val);

// returns total list length (length = list_l)
int llen(NODE* list);

// prints list characters until the first 0 char (included)
void printl(NODE*);

// pints interger value of every char in list until NULL is reached
// only used for debug purposes
void debugl(NODE*);

// write character in list on a specified position (index)
int lwrite(NODE*, int index, int val);

/*
 * insert character in list on a specified position (index)
 * parameters:
 *   - NODE* list: first node of list
 *   - int val: character to add
 * return:
 *   - NODE*: list
 */
NODE* lappend(NODE* list, int val);

// inserts node to a specific position replacing it with the last node of the list (if available)
// structure and usage of function is similar to ldeletec
NODE* lInsertN(NODE* list, int x, int val);

// deletes node in (param)index position:
//  usage of this function is as follows: (NODE*)list = ldeletc((NODE*)list, (index)position);
//                                           (the updated list is returned)
NODE* lDeleteN(NODE* list, int index);

// get node based of passed index
// if index out of range returns NULL
NODE* lgetn(NODE*, int);

// recursive function to set all char to 0 in list
void leraseall(NODE*);

/*
 * deallocates all nodes of the list
 * parameters:
 *   - NODE* node: first node of list
 * return: none
 */
void lDeleteAll(NODE* node);

#endif
