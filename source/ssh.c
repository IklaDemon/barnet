#include "ssh.h"

/*
 *  ;
 */
void SSH(char* ip, char* KEntryTitle, MASTER_KEY* mk)
{
    char* command = (char*) calloc (1024, sizeof (char));
    char* username = NULL;
    char* password = NULL;
    if (SSH_GET_CREDENTIALS(KEntryTitle, username, password, mk) != 0) {
        printf("\nerror while retriving credentials");
    } else {
        //sprintf(command, "start powershell.exe executables\\plink.exe -ssh %s@%s -pw %s", username, ip, password);
        sprintf(command, "start executables\\putty.exe -ssh %s@%s -pw %s", username, ip, password);
        //printf("\npassword: %s", password);
        system(command);
        for (int i = 0; i < 200; i++) {
            username[i] = 0;
            password[i] = 0;
        }
    }
    free (command);
    free (username);
    free (password);
}

/*
 *  questa funzione fa troppo schifo, devo rifarlo completamente (devo inoltre controllare come arriva l'input KEntryTitle)
 */
int SSH_GET_CREDENTIALS(char* KEntryTitle, char* username, char* password, MASTER_KEY* mk)
{
    // new modifications ---
    int i, commandLength, KEntryTitleSize = 0;
    const int processOuputSize = 1024, outputLength = 50;
    char kpOut[processOuputSize];
    char* command;
    FILE* pPipe;
    commandLength = strlen("executables\\keepass\\KPScript.exe -c:GetEntryString \"passwords\\p.kdbx\" -pw:") + MASTER_KEY_MAX_SIZE;
    // debug
    printf ("command length: %d", commandLength);
    for (int tmp = 0; tmp < processOuputSize; tmp++) kpOut[tmp] = 0;
    username = (char*) calloc (outputLength, sizeof (char));
    password = (char*) calloc (outputLength, sizeof (char));
    command = (char*) calloc (commandLength, sizeof (char));
    for (int tmp = 0; tmp < commandLength; tmp++) command[tmp] = 0;

    for (KEntryTitleSize = 0; KEntryTitle[KEntryTitleSize] != 0; KEntryTitleSize++);

    sprintf(command, "executables\\keepass\\KPScript.exe -c:GetEntryString \"passwords\\p.kdbx\" -pw:%s", mk->key);
    if ((pPipe = _popen(command, "rt")) == NULL)
    {
        puts("\nerror while executing KPScript.exe");
        free (username);
        free (password);
        free (command);
        return 1;
    }
    fgets(kpOut, processOuputSize, pPipe);
    for (i = 0; kpOut[i] != 0 && i < processOuputSize; i++)
        if (kpOut[i] == '\n')
        {
            kpOut[i] = 0;
            break;
        }
    if (strcmp (kpOut, "E: The master key is invalid!") == 0)
    {
        printf ("\n%s", kpOut);
        free (username);
        free (password);
        free (command);
        return 1;
    }
    _pclose(pPipe);

    // cleans array kpOut
    for (i = 0; i < processOuputSize; i++) kpOut[i] = 0;

    commandLength = strlen("executables\\keepass\\KPScript.exe -c:GetEntryString \"passwords\\p.kdbx\" -pw: -Field:Password -ref-Title:\"\"") + MASTER_KEY_MAX_SIZE;

    sprintf(command, "executables\\keepass\\KPScript.exe -c:GetEntryString \"passwords\\p.kdbx\" -pw:%s -Field:Password -ref-Title:\"%s\"", mk->key, KEntryTitle);
    if ((pPipe = _popen(command, "rt")) == NULL)
    {
        puts("\nerror while executing KPScript.exe");
        return 1;
    }
    fgets(kpOut, processOuputSize, pPipe);
    for (i = 0; kpOut[i] != '\n' && i < processOuputSize; i++)
        password[i] = kpOut[i];
    password[i] = 0;
    // printf ("\npassword = {%s}", password);
    _pclose(pPipe);

    for (i = 0; i < processOuputSize; i++)
    {
        command[i] = 0;
        kpOut[i] = 0;
    }

    sprintf(command, "executables\\keepass\\KPScript.exe -c:GetEntryString \"passwords\\p.kdbx\" -pw:%s -Field:UserName -ref-Title:\"%s\"", mk->key, KEntryTitle);
    if ((pPipe = _popen(command, "rt")) == NULL)
    {
        puts("\nerror while executing KPScript.exe");
        return 1;
    }
    fgets(kpOut, processOuputSize, pPipe);
    for (i = 0; kpOut[i] != '\n' && i < processOuputSize; i++)
        username[i] = kpOut[i];
    username[i] = 0;
    // printf ("\nusername = {%s}", username);
    _pclose(pPipe);

    free (command);
    return 0;
}

/*
 * loads file 'devices-db.txt' in RECORDS array
 *
 * parameters:
 *  - int* nRecords : return of the number of records
 *  - DEVICES_DB_DATA* dbData : return of file information
 * 
 * return:
 *   - NULL : error
 *   - RECORDS* : array of records
 */
RECORDS* SSH_LOAD_HOSTS(int* nRecords, DEVICES_DB_DATA* dbData)
{
    RECORDS* records;
    if (checkDevicesDB(".\\data\\devices-db.csv") == 1)
    {
        printf ("ssh.c:SSH_LOAD_HOSTS: checkDevicesDB error");
        return NULL;
    }
    *nRecords = getDevicesDBLines (".\\data\\devices-db.csv");
    printf ("\nnum of records: %d\n", *nRecords);
    if (*nRecords == -1) {
        printf ("\nError in devices-db.txt");
        return NULL;
    }
    records = (RECORDS*) malloc (*nRecords * sizeof(RECORDS));
    if (records == NULL) {
        printf("\nerror while allocating memory for records");
        return NULL;
    }
    if (getDevicesDBData (dbData) == -1) {
        printf("\nerror retriving devices-db.txt data");
        free (records);
        return NULL;
    }
    cleanRecords (records, *nRecords);
    if (loadRecordsFromFile (records) != 0) {
        printf("\nerror while loading hosts");
        free (records);
        return NULL;
    }
    return records;
}

/*
 *  Print hosts loaded.
 *  param:
 *   - records: host loaded
 *   - num_of_records: as is
 *  return: void
 */
void SSH_PRINT_HOSTS(RECORDS* records, int num_of_records)
{
    for (int i = 0; i < num_of_records; i++) {
        printf("\n[%d][%s]", records[i].index, records[i].string);
    }
}

/*
 *  de-allocates memory
 *  param:
 *   - records: host loaded
 */
void SSH_FREE_HOSTS(RECORDS* records) { free(records); }

/*
 *  check if input keys are valid.
 *  param:
 *   - char_hit: as is
 *  return:
 *   - 1 if key is valid
 *   - 0 if key is not valid
 */
int isValidKey(int char_hit)
{
    if ((char_hit >= '0' && char_hit <= '9') || (char_hit >= 'A' && char_hit <= 'Z') || (char_hit >= 'a' && char_hit <= 'z'))
        return 1;
    switch (char_hit)
    {
    case '-':
        return 1;
    case '.':
        return 1;
    case ' ':
        return 1;
    default:
        return 0;
    }
}

/*
 *  as is.
 *  param:
 *   - array: as is
 *   - size: as is
 *  return: void
 */
void clean_array(char* array, int size)
{
    for (int i = 0; i < size; i++)
        array[i] = 0;
}

/*
 *  as is.
 *  param:
 *   - records: as is
 *   - num_of_records: as is
 *  return: void
 */
void clean_records_found(RECORDS_FOUND* records, int num_of_records)
{
    for (int i = 0; i < num_of_records; i++) {
        records[i].index = 0;
        records[i].isSelected = 0;
        for (int j = 0; j < LINE_MAX_SIZE; j++) {
            records[i].string[j] = 0;
        }
    }
}

/*
 *  Naive Pattern Searching algorithm
 *  param:
 *   - txt
 *   - pat
 *  return:
 *   - 1 if ok
 *   - 0 if not ok
 */
int naive_pattern_matching(char* txt, char* pat)
{
    if (pat == NULL)
        return 0;
    int M = strlen(pat);
    int N = strlen(txt);
    // A loop to slide pat[] one by one
    for (int i = 0; i <= N - M; i++) {
        int j;
        // For current index i, check for pattern match
        for (j = 0; j < M; j++) {
            if (txt[i + j] != pat[j]) {
                break;
            }
        }
        // If pattern matches at index i
        if (j == M) {
            return 1;
        }
    }
    return 0;
}

/*
 *  ;
 *  param:
 *   - records: as is
 *   - records_found: as is
 *   - num_of_records: as is
 *  return:
 *   - number of matches found
 */
int find_hosts_1(RECORDS* records, RECORDS_FOUND* records_found, int num_of_records, char* pattern)
{
    int index_records_found = 0;
    clean_records_found(records_found, num_of_records);
    for (int i = 0; i < num_of_records; i++) {
        if (naive_pattern_matching(records[i].string, pattern) == 1) {
            strcpy(records_found[index_records_found].string, records[i].string);
            index_records_found++;
        }
    }
    if (index_records_found > 0)
        records_found[0].isSelected = 1;
    return index_records_found;
}

/*
 *  ;
 *  param:
 *   - records: as is
 *   - records_found: as is
 *   - num_of_records: as is
 *  return:
 *   - number of matches found
 */
int find_hosts_2(RECORDS_FOUND* rec_source, RECORDS_FOUND* rec_dest, int num_of_records, char* pattern)
{
    int index_rec_dest = 0;
    clean_records_found(rec_dest, num_of_records);
    for (int i = 0; i < num_of_records; i++) {
        if (naive_pattern_matching(rec_source[i].string, pattern) == 1) {
            strcpy(rec_dest[index_rec_dest].string, rec_source[i].string);
            index_rec_dest++;
        }
    }
    if (index_rec_dest > 0)
        rec_dest[0].isSelected = 1;
    return index_rec_dest;
}

int getSelectedPos (RECORDS_FOUND* rec, int nRecords)
{
    int is_selected_pos = 0;
    if (nRecords > 0) {
        for (is_selected_pos = 0; rec[is_selected_pos].isSelected != 1; is_selected_pos++)
            ;
        return is_selected_pos;
    } else {
        return -1;
    }
}

void getCursorPosition(HANDLE screen, int* x, int* y)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo (screen, &info);
    *x = info.dwCursorPosition.X;
    *y = info.dwCursorPosition.Y;
}

void setCursorPosition(HANDLE screen, int x, int y)
{
    COORD coord = {x, y};
    SetConsoleCursorPosition(screen,coord);
}

/*
 *  gets number of columns and rows in the current terminal
 */
void getWindowSize (HANDLE screen, int* width, int* height)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(screen, &info);
    *width = info.dwSize.X;
    *height = info.dwSize.Y;
}

void copyLineToBuffer(CHAR_INFO* buffer, char* strLine, int nLine, int nBufferColumns, int nBufferLines)
{
    int start = nLine * nBufferColumns;
    int end = (nLine * nBufferColumns) + nBufferColumns - 1;
    int strLength = strlen(strLine);
    for (int i = start, j = 0; i < end && j < strLength; i++, j++) {
        buffer[i].Char.AsciiChar = strLine[j];
        buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    }
}

/*
 *  sets alla buffer characters to specific value (' ')
 */
void cleanBuffer (CHAR_INFO* buffer, int len) {
    for(int i = 0; i < len; i++) {
        buffer[i].Char.AsciiChar = ' ';
        buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE/* | FOREGROUND_INTENSITY*/;
    }
}

void generateHeader (CHAR_INFO* buffer, int bufferC, int bufferL, LIST* pattern1, LIST* pattern2) {
    char pattern[PATTERN_MAX_SIZE] = {0};
    char* bufferLine = (char*) calloc (bufferC, sizeof (char));
    if (pattern1 != NULL) {
        LIST_TO_ARRAY (pattern1, pattern);
        sprintf (bufferLine, "/ %s", pattern);
        copyLineToBuffer (buffer, bufferLine, 0, bufferC, bufferL);
    } else {
        sprintf (bufferLine, "/ ");
        copyLineToBuffer (buffer, bufferLine, 0, bufferC, bufferL);
    }
    if (pattern2 != NULL) {
        LIST_TO_ARRAY (pattern2, pattern);
        sprintf (bufferLine, "/ %s", pattern);
        copyLineToBuffer (buffer, bufferLine, 1, bufferC, bufferL);
    }
    free (bufferLine);
}

void myLineFormat (char* bufferLine, RECORDS_FOUND* rec, DEVICES_DB_DATA* dbData) {
    int recIndex = 0, bufferLineIndex = 0, i;
    if (rec->isSelected == 1) {
        bufferLine[0] = ' ';
        bufferLine[1] = '-';
        bufferLine[2] = '>';
        bufferLine[3] = ' ';
        bufferLine[4] = '|';
    } else {
        bufferLine[0] = ' ';
        bufferLine[1] = ' ';
        bufferLine[2] = ' ';
        bufferLine[3] = ' ';
        bufferLine[4] = '|';
    }
    bufferLine[5] = ' ';
    bufferLineIndex = 6;

    for (i = 0; i < dbData->deviceNameMaxSize; i++) {
        if (rec->string[recIndex] != ' ') {
            bufferLine[bufferLineIndex] = rec->string[recIndex];
            recIndex++;
            bufferLineIndex++;
        } else {
            bufferLine[bufferLineIndex] = ' ';
            bufferLineIndex++;
        }
    }
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = '|';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    recIndex++;

    for (i = 0; i < dbData->IPMaxSize; i++) {
        if (rec->string[recIndex] != ' ') {
            bufferLine[bufferLineIndex] = rec->string[recIndex];
            recIndex++;
            bufferLineIndex++;
        } else {
            bufferLine[bufferLineIndex] = ' ';
            bufferLineIndex++;
        }
    }
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = '|';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    recIndex++;

    for (i = 0; i < dbData->siteMaxSize; i++) {
        if (rec->string[recIndex] != ' ') {
            bufferLine[bufferLineIndex] = rec->string[recIndex];
            recIndex++;
            bufferLineIndex++;
        } else {
            bufferLine[bufferLineIndex] = ' ';
            bufferLineIndex++;
        }
    }
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = '|';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    recIndex++;

    for (i = 0; i < dbData->platformMaxSize; i++) {
        if (rec->string[recIndex] != ' ') {
            bufferLine[bufferLineIndex] = rec->string[recIndex];
            recIndex++;
            bufferLineIndex++;
        } else {
            bufferLine[bufferLineIndex] = ' ';
            bufferLineIndex++;
        }
    }
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = '|';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    recIndex++;

    for (i = 0; i < dbData->verMaxSize; i++) {
        if (rec->string[recIndex] != ' ') {
            bufferLine[bufferLineIndex] = rec->string[recIndex];
            recIndex++;
            bufferLineIndex++;
        } else {
            bufferLine[bufferLineIndex] = ' ';
            bufferLineIndex++;
        }
    }
    bufferLine[bufferLineIndex] = ' ';
    bufferLineIndex++;
    bufferLine[bufferLineIndex] = '|';
    bufferLineIndex++;
}

void generateBody (CHAR_INFO* buffer, int bufferC, int bufferL, RECORDS_FOUND* rec, int recN, DEVICES_DB_DATA* dbData) {
    int selectedPos = 0, startRec, endRec;
    char* bufferLine;
    if (recN > 0) {
        bufferLine = (char*) calloc (bufferC, sizeof (char));
        selectedPos = getSelectedPos (rec, recN);
        if (selectedPos == - 1) selectedPos = 0;
        startRec = selectedPos;
        if ((selectedPos + bufferL - 4) < recN) {
            endRec = selectedPos + bufferL - 4;
        } else {
            endRec = recN;
        }
        for (int i = startRec, j = 0; i < endRec; i++, j++) {
            /*if (rec[i].isSelected == 1) {
                sprintf(bufferLine, " -> %s", rec[i].string);
            } else {
                sprintf(bufferLine, "    %s", rec[i].string);
            }*/
            myLineFormat (bufferLine, &rec[i], dbData);
            copyLineToBuffer(buffer, bufferLine, j + 2, bufferC, bufferL);
        }
        free (bufferLine);
    }
}

void generateFooter (CHAR_INFO* buffer, int bufferC, int bufferL) {
    char* bufferLine = (char*) calloc (bufferC, sizeof (char));
    sprintf (bufferLine, "Number of Records Found: TODO");
    copyLineToBuffer(buffer, bufferLine, bufferL - 1, bufferC, bufferL);
}

/*
 *  printing and formatting of records found based on pattern
 */
void print_records_found(RECORDS_FOUND* rec, int num_rec, LIST* pattern_1, LIST* pattern_2, HANDLE screen, DEVICES_DB_DATA* dbData)
{
    int termW, termH;
    CHAR_INFO* buffer;

    getWindowSize (screen, &termW, &termH);
    buffer = calloc (termW * termH, sizeof (CHAR_INFO));
    cleanBuffer (buffer, termW * termH);
    generateHeader (buffer, termW, termH, pattern_1, pattern_2);
    generateBody (buffer, termW, termH, rec, num_rec, dbData);
    generateFooter (buffer, termW, termH);
    WriteConsoleOutput (screen, buffer, (COORD){termW, termH}, (COORD){0, 0}, &(SMALL_RECT){0, 0, termW - 1, termH - 1});
    free (buffer);
}

void getIPFromRecord (char* record, char* IP) {
    int startIP = 0, endIP = 0;

    for (startIP = 0; record[startIP] != ' '; startIP++)
        ;
    startIP++;
    for (endIP = startIP; record[endIP] != ' '; endIP++)
        ;
    for (int i = 0; startIP < endIP; startIP++, i++) IP[i] = record[startIP];
}

void getKPTitle (char* record, char* KPTitle) {
    int start = 0, spaces;
    for (start = 0, spaces = 0; spaces < 5; start++) if (record[start] == ' ') spaces++;
    for (int i = 0; record[start] != ' ' && record[start] != '\n'; start++, i++) KPTitle[i] = record[start];
}

/*void getKPPassN (char* record, char* KPPassN) {
    int start = 0, spaces;
    for (start = 0, spaces = 0; spaces < 6; start++) if (record[start] == ' ') spaces++;
    *KPPassN = record[start];
}*/

/*
 *  lots of stuff
 */
int SSH_INPUT_AND_FIND_PATTERN(RECORDS* records, int num_of_records, DEVICES_DB_DATA* dbData, MASTER_KEY* mk)
{
    int flag_second_search_enabled = 0;
    int is_selected_pos;
    int flag_exit = 0;
    int char_hit;
    int n_records_found_1;
    int n_records_found_2;
    int cursorX, cursorY;
    char IP[15] = {0};
    char KPTitle[64] = {0};
    char array_pattern[PATTERN_MAX_SIZE];
    LIST* list_pattern_1 = LIST_INIT();
    LIST* list_pattern_2 = LIST_INIT();
    RECORDS_FOUND* records_found_1;
    RECORDS_FOUND* records_found_2;

    HANDLE winA;
    HANDLE winstd;

    winstd = GetStdHandle (STD_OUTPUT_HANDLE);
    winA = CreateConsoleScreenBuffer (
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    SetConsoleActiveScreenBuffer (winA);

    records_found_1 = (RECORDS_FOUND*)calloc(num_of_records, sizeof(RECORDS_FOUND));
    records_found_2 = (RECORDS_FOUND*)calloc(num_of_records, sizeof(RECORDS_FOUND));
    clean_array(array_pattern, PATTERN_MAX_SIZE);
    n_records_found_1 = find_hosts_1(records, records_found_1, num_of_records, "");
    print_records_found(records_found_1, n_records_found_1, list_pattern_1, NULL, winA, dbData);
    setCursorPosition(winA, 2, 0);

    while (!flag_exit) {
        switch (char_hit = getch()) {
        // ENTER KEY
        case 13:
            if (flag_second_search_enabled == 0)
              {
                is_selected_pos = getSelectedPos (records_found_1, n_records_found_1);
                getIPFromRecord (records_found_1[is_selected_pos].string, IP);
                getKPTitle (records_found_1[is_selected_pos].string, KPTitle);
              }
            else
              {
                is_selected_pos = getSelectedPos (records_found_2, n_records_found_2);
                //printf ("\nYour text is: [%s]", records_found_2[is_selected_pos].string);
                getIPFromRecord (records_found_2[is_selected_pos].string, IP);
                getKPTitle (records_found_2[is_selected_pos].string, KPTitle);
              }
            SSH (IP, KPTitle, mk);
            flag_exit = 1;
            break;
        // ESCAPE KEY
        case 27:
            if (flag_second_search_enabled == 0)
                flag_exit = 1;
            else {
                flag_second_search_enabled = 0;
                LIST_DELETE(list_pattern_2);
                //system("CLS");
                print_records_found(records_found_1, n_records_found_1, list_pattern_1, NULL, winA, dbData);
            }
            break;
        // DELETE KEY
        case 8:
            if (list_pattern_1->index > 0) {
                if (flag_second_search_enabled == 0) {
                    LIST_INDEX_LEFT(list_pattern_1);
                    LIST_DEL_CHAR(list_pattern_1);
                    LIST_TO_ARRAY(list_pattern_1, array_pattern);
                    n_records_found_1 = find_hosts_1(records, records_found_1, num_of_records, array_pattern);
                    print_records_found(records_found_1, n_records_found_1, list_pattern_1, NULL, winA, dbData);
                } else {
                    LIST_INDEX_LEFT(list_pattern_2);
                    LIST_DEL_CHAR(list_pattern_2);
                    LIST_TO_ARRAY(list_pattern_2, array_pattern);
                    n_records_found_2 = find_hosts_2(records_found_1, records_found_2, n_records_found_1, array_pattern);
                    print_records_found(records_found_2, n_records_found_2, list_pattern_1, list_pattern_2, winA, dbData);
                }
            }
            break;
        // ARROWS FIRST CHARACTER
        case 224:
            char_hit = getch();
            switch (char_hit) {
            // UP ARROW
            case 72:
                if (flag_second_search_enabled == 0) {
                    is_selected_pos = getSelectedPos (records_found_1, n_records_found_1);
                    if (is_selected_pos > 0) {
                        records_found_1[is_selected_pos].isSelected = 0;
                        is_selected_pos--;
                        records_found_1[is_selected_pos].isSelected = 1;
                        print_records_found(records_found_1, n_records_found_1, list_pattern_1, NULL, winA, dbData);
                    }
                } else {
                    is_selected_pos = getSelectedPos (records_found_2, n_records_found_2);
                    if (is_selected_pos > 0) {
                        records_found_2[is_selected_pos].isSelected = 0;
                        is_selected_pos--;
                        records_found_2[is_selected_pos].isSelected = 1;
                        print_records_found(records_found_2, n_records_found_2, list_pattern_1, list_pattern_2, winA, dbData);
                    }
                }
                break;
            // DOWN ARROW
            case 80:
                if (flag_second_search_enabled == 0) {
                    is_selected_pos = getSelectedPos (records_found_1, n_records_found_1);
                    if (is_selected_pos < (n_records_found_1 - 1)) {
                        records_found_1[is_selected_pos].isSelected = 0;
                        is_selected_pos++;
                        records_found_1[is_selected_pos].isSelected = 1;
                        print_records_found(records_found_1, n_records_found_1, list_pattern_1, NULL, winA, dbData);
                    }
                } else {
                    is_selected_pos = getSelectedPos (records_found_2, n_records_found_2);
                    if (is_selected_pos < (n_records_found_2 - 1)) {
                        records_found_2[is_selected_pos].isSelected = 0;
                        is_selected_pos++;
                        records_found_2[is_selected_pos].isSelected = 1;
                        print_records_found(records_found_2, n_records_found_2, list_pattern_1, list_pattern_2, winA, dbData);
                    }
                }
                break;
            // LEFT ARROW
            case 75:
                LIST_INDEX_LEFT(list_pattern_1);
                break;
            // RIGHT ARROW
            case 77:
                LIST_INDEX_RIGHT(list_pattern_1);
                break;
            default:
                break;
            }
            break;
        case '/':
            if (flag_second_search_enabled == 0) {
                flag_second_search_enabled = 1;
                n_records_found_2 = find_hosts_2(records_found_1, records_found_2, n_records_found_1, "");
                print_records_found(records_found_1, n_records_found_1, list_pattern_1, list_pattern_2, winA, dbData);
            }
            break;
        // DEFAULT CASE
        default:
            if (isValidKey(char_hit) && LIST_LEN(list_pattern_1) < (PATTERN_MAX_SIZE - 1) && LIST_LEN(list_pattern_2) < (PATTERN_MAX_SIZE - 1)) {
                if (flag_second_search_enabled == 0) {
                    LIST_ADD_CHAR(list_pattern_1, char_hit);
                    LIST_TO_ARRAY(list_pattern_1, array_pattern);
                    n_records_found_1 = find_hosts_1(records, records_found_1, num_of_records, array_pattern);
                    print_records_found(records_found_1, n_records_found_1, list_pattern_1, NULL, winA, dbData);
                } else {
                    LIST_ADD_CHAR(list_pattern_2, char_hit);
                    LIST_TO_ARRAY(list_pattern_2, array_pattern);
                    n_records_found_2 = find_hosts_2(records_found_1, records_found_2, n_records_found_1, array_pattern);
                    print_records_found(records_found_2, n_records_found_2, list_pattern_1, list_pattern_2, winA, dbData);
                }
            }
            break;
        }
        if (flag_second_search_enabled == 0) {
            cursorY = 0;
            cursorX = list_pattern_1->index + 2;
            setCursorPosition(winA, cursorX, cursorY);
        } else {
            cursorY = 1;
            cursorX = list_pattern_2->index + 2;
            setCursorPosition(winA, cursorX, cursorY);
        }
    }

    SetConsoleActiveScreenBuffer (winstd);
    CloseHandle (winA);
    LIST_DELETE(list_pattern_1);
    LIST_DELETE(list_pattern_2);
    free(list_pattern_1);
    free(list_pattern_2);
    free(records_found_1);
    free(records_found_2);
    return 0;
}


int test_screen()
{
    HANDLE winA;
    HANDLE winstd;
    CHAR_INFO* buffer;
    int termW, termH;

    winstd = GetStdHandle(STD_OUTPUT_HANDLE);
    winA = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    SetConsoleActiveScreenBuffer(winA);

    getWindowSize(winA, &termW, &termH);
    buffer = calloc((termW * termH), sizeof(CHAR_INFO));

    // body

    for(int i = 0; i < (termW * termH); i++)
    {
        buffer[i].Char.AsciiChar = '.';
        buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
    }

    WriteConsoleOutput(winA, buffer, (COORD){termW, termH}, (COORD){0, 0}, &(SMALL_RECT){0, 0, termW - 1, termH - 1});

    getch();
    // end of body

    free(buffer);
    SetConsoleActiveScreenBuffer(winstd);
    CloseHandle(winA);
    return 0;
}

void SSH_SET_MASTER_KEY (MASTER_KEY* mk) {
    int exitFlag = 0, keyIndex = 0;
    char charHit;
    SSH_DELETE_MASTER_KEY (mk);
    printf ("\nPassword: ");
    while (!exitFlag) {
        charHit = getch (); 
        switch (charHit) {
            case 13:
                exitFlag = 1;
                mk->keySet = 1;
                break;
            case 27:
                exitFlag = 1;
                SSH_DELETE_MASTER_KEY (mk);
                keyIndex = 0;
                break;
            case 8:
                SSH_DELETE_MASTER_KEY (mk);
                keyIndex = 0;
                break;
            default:
                if (keyIndex < MASTER_KEY_MAX_SIZE - 1) {
                    mk->key[keyIndex] = charHit;
                    keyIndex++;
                }
                break;
        }
    }
    //printf ("\npass   : [%s]", mk->key);
    //printf ("\nkeySet : [%d]", mk->keySet);
}

void SSH_DELETE_MASTER_KEY (MASTER_KEY* mk) {
    int i;
    for (i = 0; i < MASTER_KEY_MAX_SIZE; i++) {
        mk->key[i] = 0;
    }
    mk->keySet = 0;
}

/*
 *  starts "Device Filterer" functionality and handles console screen buffer...
 *
 *  parameters:
 *   - RECORDS* rec : initialized RECORDS* structure
 *   - int recN : number of records inside rec
 *   - DEVICES_DB_DATA* dbData : data related to the devices DB (csv)
 *   - MASTER_KEY* mk : pointer to object MASTER_KEY
 * 
 *  return:
 *   - 1 : recN == -1
 *   - 2 : mk->keySet == 0
 *   - 0 : everything ok
 */
int SSH_COMMAND (RECORDS* rec, int recN, DEVICES_DB_DATA* dbData, MASTER_KEY* mk) {
    if (recN == -1)
    {
        printf ("\nerror in 'devices-db.txt'\nPlease check file in order to use ssh function");
        return 1;
    }
    if (mk->keySet == 0)
    {
        printf ("\nmaster key not set\nuse 'set-key' to set master key");
        SSH_SET_MASTER_KEY (mk);
        return 2;
    }
    /*printf("\nssh exit status: %d", */SSH_INPUT_AND_FIND_PATTERN(rec, recN, dbData, mk)/*)*/;
    return 0;
}
