#ifndef SSH_H
#define SSH_H

#include "lib.h"
#include "devices-db.h"

#define PATTERN_MAX_SIZE 150
#define MASTER_KEY_MAX_SIZE 64


typedef struct {
    char string[LINE_MAX_SIZE];
    int index;
    int isSelected;
} RECORDS_FOUND;


typedef struct {
    char key[MASTER_KEY_MAX_SIZE];
    int keySet;
} MASTER_KEY;

void SSH(char* ip, char* KEntryTitle, MASTER_KEY* mk);
int SSH_GET_CREDENTIALS(char* KEntryTitle, char* username, char* password, MASTER_KEY* mk);
RECORDS* SSH_LOAD_HOSTS(int* nRecords, DEVICES_DB_DATA* dbData);
void SSH_PRINT_HOSTS (RECORDS* records, int num_of_records);
void SSH_FREE_HOSTS (RECORDS* records);
int SSH_INPUT_AND_FIND_PATTERN(RECORDS* records, int num_of_records, DEVICES_DB_DATA* dbData, MASTER_KEY* mk);
void SSH_SET_MASTER_KEY (MASTER_KEY* mk);
void SSH_DELETE_MASTER_KEY (MASTER_KEY* mk);
int SSH_COMMAND (RECORDS* rec, int recN, DEVICES_DB_DATA* dbData, MASTER_KEY* mk);

#endif
