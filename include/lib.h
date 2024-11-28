#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

/*
 * standard libraries
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * windows libraries
 */
#include <winsock.h>
#include <windows.h>
#include <conio.h>
#include <process.h>

/*
 * my libraries
 */
#include <list.h>

/*
 * defines
 */
#define D_COMMAND_LEN 100
#define D_X_OFFSET 2

typedef struct
{
    int x;
    int y;
} struct_window;

typedef struct
{
    char command[D_COMMAND_LEN];
    int aging_time;
} struct_history;

void
getXY (int* x, int* y);
void
gotoXY (int XPos, int YPos);

int f_length(char*);
char** f_alloc_char_matrix(int, int);
void f_free_matrix(char**, int);
void f_clean_matrix(char**, int, int);
void f_flush_stdin();
void f_format_mac(char*, int);
void f_get_format1(char*, char*, char);
void f_get_format3(char*, char*);
int f_is_mac_address(char*);
int f_is_format1(char*);
int f_is_format2(char*);
int f_is_format3(char*);
int f_check_separators_format1(char*);
int f_check_separators_format3(char*);
int f_check_hexa_format1(char*);
int f_check_hexa_format2(char*);
int f_check_hexa_format3(char*);
void f_to_lower_case(char*);
void f_to_upper_case(char*);
int f_get_oldest_command_index(struct_history*);
void f_increase_aging(struct_history*);
int f_get_index_by_aging(struct_history*, int);
int f_is_history_loaded(struct_history*);

int f_is_subnet(char*);
void f_get_subnet_data(char*);

/*
 * function that filters key pressed
 */
int f_is_key(int char_hit);

/*
 * wrote by chatgpt
 */
int is_ip(char* ip);

/*
 * 
 */
int isPort(char* str_port, int* port);

/*
 * 
 */
int isSubnet(char* string);

/*
 * 
 */
char** getIPs(char* subnet, int* ips_len);

//debug functions
void f_print_history(struct_history*);
void f_print_matrix(char **);

#endif // LIB_H_INCLUDED
