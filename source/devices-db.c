#include <stdio.h>
#include <string.h>
#include "devices-db.h"

/*
 *  get number of lines inside: 'devices-db.csv'
 *
 *  parameters:
 *   - none
 * 
 *  return:
 *   - -1 for error
 *   - lines: number of lines
 */
int getDevicesDBLines (char* path)
{
    FILE* file;
    int lines = 0;
    char ch;
    file = fopen (path, "r");
    if (file == NULL)
    {
        printf ("\nError while opening file");
        return -1;
    }
    while ((ch = fgetc (file)) != EOF)
    {
        if (ch == '\n') lines++;
    }
    fclose (file);
    return lines + 1;
}

/*
 *  check number of ',' for each line in fileName, it must be NUMBER_OF_COMMAS. otherwise returns -1.
 *
 *  parameters:
 *   - char* fileName : name of csv file
 * 
 *  return:
 *   - -1 for error
 *   - nLines: number of lines
 */
int checkNColumns (char* fileName)
{
    FILE *file = fopen (fileName, "r");
    if (file == NULL)
    {
        printf ("Error while opening %s", fileName);
        return -1;
    }
    char ch;
    int nLines = 0;
    int nSeparator = 0;
    int currentLine = 1;
    while ((ch = fgetc (file)) != EOF)
    {
        if (ch == ',')
        {
            nSeparator++;
        }
        else if (ch == '\n')
        {
            if (nSeparator != NUMBER_OF_COMMAS)
            {
                printf ("Error in line %d\n", currentLine);
                fclose (file);
                return -1;
            }
            nLines++;
            currentLine++;
            nSeparator = 0;
        }
    }
    if (nSeparator != NUMBER_OF_COMMAS && nSeparator != 0)
    {
        printf ("Error in line %d\n", currentLine);
        fclose (file);
        return -1;
    }
    if (nSeparator == NUMBER_OF_COMMAS) {
        nLines++;
    }
    fclose (file);
    return nLines;
}

/*
 *  cleans array of records setting everything to 0
 *  
 *  parameters:
 *   - RECORDS* records : array of records
 *   - int nRecords : numero di records
 * 
 *  return:
 *   - void : nada
 */
void cleanRecords (RECORDS* records, int nRecords)
{
    for (int i = 0; i < nRecords; i++)
    {
        records[i].index = 0;
        for (int j = 0; j < LINE_MAX_SIZE; j++)
        {
            records[i].string[j] = 0;
        }
    }
}

/*
 *  loads each line in "devices-db.csv" in a RECORDS array (previously allocated on Heap)
 *
 *  parameters:
 *   - RECORDS* records : array di records
 * 
 *  return:
 *   - 1 : error
 *   - 0 : ok
 */
int loadRecordsFromFile (RECORDS* records)
{
    FILE* file;
    char buffer[LINE_MAX_SIZE];
    int count = 0;

    file = fopen (".\\data\\devices-db.csv", "r");
    if (file == NULL)
    {
        printf("Error while opening 'devices-db.csv' file");
        return 1;
    }
    while (fgets (buffer, LINE_MAX_SIZE, file))
    {
        buffer[strcspn (buffer, "\n")] = 0;
        strcpy (records[count].string, buffer);
        records[count].index = count;
        count++;
    }
    return 0;
}

/*
 * get the biggest length (checking every line) of each column in devices-db.csv
 * 
 * parameters:
 *  - DEVICES_DB_DATA* dbData : must be already allocated
 * 
 * return:
 *  - 0 : ok
 *  - -1 : error
 */
int getDevicesDBData (DEVICES_DB_DATA* dbData)
{
    char ch;
    int column = 0, cnt = 0;
    FILE *file = fopen (".\\data\\devices-db.csv", "r");
    if (file == NULL)
    {
        printf ("Error while opening devices-db.csv");
        return -1;
    }

    dbData->deviceNameMaxSize = 0;
    dbData->IPMaxSize = 0;
    dbData->siteMaxSize = 0;
    dbData->platformMaxSize = 0;
    dbData->verMaxSize = 0;

    while ((ch = fgetc (file)) != EOF)
    {
        if (ch == ',')
        {
            switch (column)
            {
                case 0:
                    if (cnt > dbData->deviceNameMaxSize) dbData->deviceNameMaxSize = cnt;
                    break;
                case 1:
                    if (cnt > dbData->IPMaxSize) dbData->IPMaxSize = cnt;
                    break;
                case 2:
                    if (cnt > dbData->siteMaxSize) dbData->siteMaxSize = cnt;
                    break;
                case 3:
                    if (cnt > dbData->platformMaxSize) dbData->platformMaxSize = cnt;
                    break;
                case 4:
                    if (cnt > dbData->verMaxSize) dbData->verMaxSize = cnt;
                    break;
                default:
                    break;
            }
            column++;
            cnt = 0;
        }
        else if (ch == '\n')
        {
            column = 0;
            cnt = 0;
        } else
        {
            cnt++;
        }
    }
    return 0;
}

/*
 * count number of characters on first column
 * 
 * parameters:
 *  - char* fileName : file name
 * 
 * return:
 *  - 1 : error
 *  - 0 : ok
 */
int checkDevicesDB (char* fileName)
{
    FILE* file;
    char ch;

    int line = 0;
    int column = 0;
    int error = 0;

    int nameLen = 0;
    int ipLen = 0;
    int siteLen = 0;
    int platformLen = 0;
    int verLen = 0;

    int kpName = 0;
    int kpEntry = 0;

    if (checkNColumns (fileName) == -1) return 1;

    file = fopen (fileName, "r");
    if (file == NULL)
    {
        printf ("\nError while opening '%s'\n", fileName);
        return -1;
    }

    while ((ch = fgetc (file)) != EOF)
    {
        switch (ch)
        {
        case '\n':
            if (nameLen > DEVICE_NAME_MAX_SIZE)
            {
                printf ("\nline %d: column 0 exceeds length limit (%d/%d)", line, nameLen, DEVICE_NAME_MAX_SIZE);
                error = 1;
            }
            if (ipLen > IP_MAX_SIZE)
            {
                printf ("\nline %d: column 1 exceeds length limit (%d/%d)", line, ipLen, IP_MAX_SIZE);
                error = 1;
            }
            if (siteLen > SITE_MAX_SIZE)
            {
                printf ("\nline %d: column 2 exceeds length limit (%d/%d)", line, siteLen, SITE_MAX_SIZE);
                error = 1;
            }
            if (platformLen > PLATFORM_MAX_SIZE)
            {
                printf ("\nline %d: column 3 exceeds length limit (%d/%d)", line, platformLen, PLATFORM_MAX_SIZE);
                error = 1;
            }
            if (verLen > VER_MAX_SIZE)
            {
                printf ("\nline %d: column 4 exceeds length limit (%d/%d)", line, verLen, VER_MAX_SIZE);
                error = 1;
            }
            if (kpName > KP_NAME)
            {
                printf ("\nline %d: column 5 exceeds length limit (%d/%d)", line, kpName, KP_NAME);
                error = 1;
            }
            if (kpEntry > KP_ENTRY_NUMBER)
            {
                printf ("\nline %d: column 6 exceeds length limit (%d/%d)", line, kpEntry, KP_ENTRY_NUMBER);
                error = 1;
            }
            line++;
            column = 0;
            nameLen     = 0;
            ipLen       = 0;
            siteLen     = 0;
            platformLen = 0;
            verLen      = 0;
            kpName      = 0;
            kpEntry     = 0;
            break;
        case ',':
            /*switch (column)
            {
            case 0:
                printf ("\nline %d column %d len %d", line, column, nameLen);
                break;
            case 1:
                printf ("\nline %d column %d len %d", line, column, ipLen);
                break;
            case 2:
                printf ("\nline %d column %d len %d", line, column, siteLen);
                break;
            case 3:
                printf ("\nline %d column %d len %d", line, column, platformLen);
                break;
            case 4:
                printf ("\nline %d column %d len %d", line, column, verLen);
                break;
            default:
                break;
            }*/
            column++;
            break;
        default:
            switch (column)
            {
            case 0:
                nameLen++;
                break;
            case 1:
                ipLen++;
                break;
            case 2:
                siteLen++;
                break;
            case 3:
                platformLen++;
                break;
            case 4:
                verLen++;
                break;
            case 5:
                kpName++;
                break;
            case 6:
                kpEntry++;
                break;
            default:
                break;
            }
            break;
        }
    }

    if (nameLen > DEVICE_NAME_MAX_SIZE)
    {
        printf ("\nline %d: column 0 exceeds length limit (%d/%d)", line, nameLen, DEVICE_NAME_MAX_SIZE);
        error = 1;
    }
    if (ipLen > IP_MAX_SIZE)
    {
        printf ("\nline %d: column 1 exceeds length limit (%d/%d)", line, ipLen, IP_MAX_SIZE);
        error = 1;
    }
    if (siteLen > SITE_MAX_SIZE)
    {
        printf ("\nline %d: column 2 exceeds length limit (%d/%d)", line, siteLen, SITE_MAX_SIZE);
        error = 1;
    }
    if (platformLen > PLATFORM_MAX_SIZE)
    {
        printf ("\nline %d: column 3 exceeds length limit (%d/%d)", line, platformLen, PLATFORM_MAX_SIZE);
        error = 1;
    }
    if (verLen > VER_MAX_SIZE)
    {
        printf ("\nline %d: column 4 exceeds length limit (%d/%d)", line, verLen, VER_MAX_SIZE);
        error = 1;
    }
    if (kpName > KP_NAME)
    {
        printf ("\nline %d: column 5 exceeds length limit (%d/%d)", line, kpName, KP_NAME);
        error = 1;
    }
    if (kpEntry > KP_ENTRY_NUMBER)
    {
        printf ("\nline %d: column 6 exceeds length limit (%d/%d)", line, kpEntry, KP_ENTRY_NUMBER);
        error = 1;
    }

    fclose (file);
    return error;
}