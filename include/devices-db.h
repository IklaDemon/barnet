#ifndef devices_db_H
#define devices_db_H

#define DEVICE_NAME_MAX_SIZE 50
#define IP_MAX_SIZE 15
#define SITE_MAX_SIZE 50
#define PLATFORM_MAX_SIZE 20
#define VER_MAX_SIZE 15

#define KP_NAME 20
#define KP_ENTRY_NUMBER 1
#define NUMBER_OF_COMMAS 6

#define LINE_MAX_SIZE (DEVICE_NAME_MAX_SIZE + IP_MAX_SIZE + SITE_MAX_SIZE + PLATFORM_MAX_SIZE + VER_MAX_SIZE + KP_NAME + KP_ENTRY_NUMBER + NUMBER_OF_COMMAS)

typedef struct {
    char string[LINE_MAX_SIZE];
    int index;
} RECORDS;

typedef struct {
    int deviceNameMaxSize;
    int IPMaxSize;
    int siteMaxSize;
    int platformMaxSize;
    int verMaxSize;
} DEVICES_DB_DATA;

int getDevicesDBLines (char* path);
int checkNColumns (char* fileName);
void cleanRecords (RECORDS* records, int nRecords);
int loadRecordsFromFile (RECORDS* records);
int getDevicesDBData (DEVICES_DB_DATA* dbData);
int checkDevicesDB (char* fileName);

#endif