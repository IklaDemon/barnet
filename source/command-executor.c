#include "command-executor.h"
#include "list.h"
#include "lib.h"
#include "ssh.h"

/*
 *
 * vorrei fare una funzione che gestisce gli input dell'utente, e quando l'utente preme "enter" il comando venga
 * eseguito chiamando la funzione opportuna
 * 
 * spostato tutto il codice main dentro userInputHandler
 *
 */

int userInputHandler ()
{
    LIST* listCommand = LIST_INIT ();
    struct_window window;
    char* arrayCommand = (char*) calloc (D_COMMAND_LEN, sizeof (char));
    char** splittedCommand = f_alloc_char_matrix (10, D_COMMAND_LEN);
    char* arraySupport;
    int i = 0, charHit = 0, flagExit = 1, nRecords;
    RECORDS* records;
    DEVICES_DB_DATA dbData;
    MASTER_KEY mk;
    mk.keySet = 0;

    checkDevicesDB (".\\data\\devices-db.csv");

    getDevicesDBData (&dbData);
    records = SSH_LOAD_HOSTS(&nRecords, &dbData);

    putchar('>');
    putchar(' ');
    while (flagExit)
    {
        charHit = getch ();
        switch (charHit)
        {
        // Enter key
        case 13:
            // elaborate the command and print the result if something has been typed;
            if (LIST_LEN(listCommand) > 0)
            {
                LIST_TO_ARRAY(listCommand, arrayCommand);
                // split command
                arraySupport = strtok(arrayCommand, " ");
                i = 0;
                while (arraySupport != NULL && i < 10)
                {
                    strcpy(splittedCommand[i], arraySupport);
                    i++;
                    arraySupport = strtok(NULL, " ");
                }

                // elaborates command and prints result
                // -----------------------------------------------------------------------------------------------------------------------------------------------
                if (strcmp(splittedCommand[0], "ex") == 0 && f_length(splittedCommand[1]) == 0)
                {
                    flagExit = 0;
                }
                else if (strcmp(splittedCommand[0], "mac") == 0)
                {
                    if (f_length(splittedCommand[1]) != 0)
                    {
                        if (f_is_mac_address(splittedCommand[1]) > 0)
                        {
                            f_format_mac(splittedCommand[1], f_is_mac_address(splittedCommand[1]));
                        }
                    }
                }
                else if (strcmp(splittedCommand[0], "net") == 0 && f_length(splittedCommand[1]) != 0)
                {
                    if (f_is_subnet(splittedCommand[1]))
                    {
                        _cprintf("\n   |%s|", splittedCommand[1]);
                        f_get_subnet_data(splittedCommand[1]);
                    }
                    getXY(&window.x, &window.y);
                }
                
                else if (strcmp(splittedCommand[0], "ssh") == 0)
                {
                    SSH_COMMAND (records, nRecords, &dbData, &mk);
                }
                else if (strcmp(splittedCommand[0], "set-key") == 0)
                {
                    SSH_SET_MASTER_KEY (&mk);
                }
                else
                {
                    printf("\n   command: \"");
                    LIST_PRINT(listCommand);
                    putchar('"');
                    printf("\n   unrecognized command");
                }
                for (i = 0; i < D_COMMAND_LEN; i++)
                    arrayCommand[i] = 0;
                f_clean_matrix(splittedCommand, 10, D_COMMAND_LEN);
                LIST_DELETE(listCommand);
            }
            if (flagExit)
            {
                putchar('\n');
                putchar('>');
                putchar(' ');
            }
            break;
        // del key
        case 8:
            if (listCommand->index > 0)
            {
                getXY(&window.x, &window.y);
                LIST_INDEX_LEFT(listCommand);
                LIST_DEL_CHAR(listCommand);
                window.x = listCommand->index + D_X_OFFSET;
                gotoXY(window.x, window.y);
                LIST_PRINT_FROM_INDEX(listCommand);
                putchar(' ');
                gotoXY(window.x, window.y);
            }
            break;
        // ESCAPE
        case 27:
            flagExit = 0;
            break;
        // Arrow first character
        case 224:
            charHit = getch ();
            switch (charHit)
            {
            // up arrow
            case 72:
                break;
            // down arrow
            case 80:
                break;
            // Left arrow
            case 75:
                LIST_INDEX_LEFT(listCommand);
                break;
            // Rigth arrow
            case 77:
                LIST_INDEX_RIGHT(listCommand);
                break;
            default:
                break;
            }
            break;
        default:
            if (f_is_key(charHit) && listCommand->index < (D_COMMAND_LEN - 1))
            {
                LIST_ADD_CHAR(listCommand, charHit);
                putchar(charHit);
                LIST_PRINT_FROM_INDEX(listCommand);
            }
            break;
        }
        getXY(&window.x, &window.y);
        window.x = listCommand->index + D_X_OFFSET;
        gotoXY(window.x, window.y);
    }

    SSH_DELETE_MASTER_KEY (&mk);
    puts("\nbye");
    return 0;
}