#include "lib.h"

/*
 *  gets relative position of x and y on current terminal window
 */
void
getXY (int* x, int* y)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(h, &bufferInfo);
    *x=bufferInfo.dwCursorPosition.X;
    *y=bufferInfo.dwCursorPosition.Y;
}


/*
 *  sets relative position of x and y on current terminal window
 */
void
gotoXY (int XPos, int YPos)
{
    COORD coord;
    coord.X = XPos; coord.Y = YPos;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

char** f_alloc_char_matrix(int x, int y)
{
    int i;
    char **matrix = (char**) calloc(x, sizeof(char*));
    for(i = 0; i < x; i++) matrix[i] = (char*) calloc(y, sizeof(char));
    return matrix;
}

void f_free_matrix(char** matrix, int x)
{
    int i;
    for(i = 0; i < x; i++) free(matrix[i]);
    free(matrix);
}

void f_clean_matrix(char** matrix, int x, int y)
{
    int i, j;
    for(i = 0; i < x; i++) for(j = 0; j < y; j++) matrix[i][j] = 0;
}

/* 
 * calcs array length
 * for loop stops when reaches '\0' or D_COMMAND_LEN (it's a define located at lib.h)
 */
int f_length(char* array)
{
    int i = 0;
    for(i = 0; *(array+i) != 0 && i < (D_COMMAND_LEN - 1); i++);
    return i;
}

/*
 *  flush stdin.
 */
void f_flush_stdin()
{
    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF);
}

/*
 * input types of mac address:
 * 1) FF-99:FF.99-FF:99   u or l   17
 * 2) FF99FF99FF99        u or l   12
 * 3) FFFF.9999.FFFF      u or l   14
 * the objective is to convert any type of a given mac ad in the second form, to then output it to the other forms.
 * output type of mac address:
 * 1) FF-99-FF-99-FF-99
 * 2) ff-99-ff-99-ff-99

 * 3) FF:99:FF:99:FF:99
 * 4) ff:99:ff:99:ff:99

 * 5) FF99FF99FF99
 * 6) ff99ff99ff99

 * 7) FFFF.9999.FFFF
 * 8) ffff.9999.ffff
 * 
 * returns formated mac address
 *
 */

void f_format_mac(char* in, int mac_ad_format)
{
    char mac_format_2[13] = {0};
    char **mac_ad_out;
    int i, j, l = 0;
    f_to_lower_case(in);
    switch(mac_ad_format)
    {
        case 1:
            for(j = 0; j <= 15; j += 3)
                for(i = j; i < (j + 2); i++)
                {
                    mac_format_2[l] = in[i];
                    l++;
                }
            break;
        case 2:
            strcpy(mac_format_2, in);
            break;
        case 3:
            for(j = 0; j <= 10; j += 5)
                for(i = j; i < (j + 4); i++)
                {
                    mac_format_2[l] = in[i];
                    l++;
                }
            break;
        default:
            break;
    }
    mac_ad_out = (char**) calloc(8, sizeof(char*));
    for(i = 0; i < 8; i++) mac_ad_out[i] = (char*) calloc(20 , sizeof(char));

    f_get_format1(mac_ad_out[0], mac_format_2, '-');
    f_to_upper_case(mac_ad_out[0]);
    f_get_format1(mac_ad_out[1], mac_format_2, '-');
    f_get_format1(mac_ad_out[2], mac_format_2, ':');
    f_to_upper_case(mac_ad_out[2]);
    f_get_format1(mac_ad_out[3], mac_format_2, ':');
    strcpy(mac_ad_out[4], mac_format_2);
    f_to_upper_case(mac_ad_out[4]);
    strcpy(mac_ad_out[5], mac_format_2);
    f_get_format3(mac_ad_out[6], mac_format_2);
    f_to_upper_case(mac_ad_out[6]);
    f_get_format3(mac_ad_out[7], mac_format_2);


    _cprintf ("\n   -----------------------------------------\n   | %-17s | %-17s |\n   -----------------------------------------", "Uppercase", "Lowercase");
    for (i = 0; i < 8; i += 2)
      _cprintf ("\n   | %-17s | %-17s |", mac_ad_out[i], mac_ad_out[i + 1]);
    _cprintf ("\n   -----------------------------------------");
    f_free_matrix (mac_ad_out, 8);

    //return mac_ad_out;
}

void f_get_format1(char *mac_ad_format_1, char *mac_ad_format_2, char format_1_separator)
{
    int i, j, l = 0;
    for(j = 0; j <= 15; j += 3)
        for(i = j; i < (j + 2); i++)
        {
            mac_ad_format_1[i] = mac_ad_format_2[l];
            l++;
        }
    for(i = 2; i <= 14; i += 3)
        mac_ad_format_1[i] = format_1_separator;
}

void f_get_format3(char *mac_ad_format_1, char *mac_ad_format_2)
{
    int i, j, l = 0;
    for(j = 0; j <= 10; j += 5)
        for(i = j; i < (j + 4); i++)
        {
            mac_ad_format_1[i] = mac_ad_format_2[l];
            l++;
        }
    mac_ad_format_1[4] = '.';
    mac_ad_format_1[9] = '.';
}

int f_is_mac_address(char* in_mac_address)
{
    int mac_ad_format;
    if(f_is_format1(in_mac_address)) mac_ad_format = 1;
    else if(f_is_format2(in_mac_address)) mac_ad_format = 2;
    else if(f_is_format3(in_mac_address)) mac_ad_format = 3;
    else mac_ad_format = 0;
    return mac_ad_format;
}

//check if mac ad is first form
int f_is_format1(char* in)
{
    int in_len = f_length(in);
    if(in_len != 17) return 0;
    f_to_lower_case(in);
    if(f_check_separators_format1(in) != 1) return 0;
    if(f_check_hexa_format1(in) != 1) return 0;
    return 1;
}

int f_is_format2(char* in)
{
    int in_len = f_length(in);
    if(in_len != 12) return 0;
    f_to_lower_case(in);
    if(f_check_hexa_format2(in) != 1) return 0;
    return 1;
}

//check if mac ad is third form
int f_is_format3(char* in)
{
    int in_len = f_length(in);
    if(in_len != 14) return 0;
    f_to_lower_case(in);
    if(f_check_separators_format3(in) != 1) return 0;
    if(f_check_hexa_format3(in) != 1) return 0;
    return 1;
}

int f_check_separators_format1(char* in)
{
    int i, flag = 1;
    for(i = 2; i <= 14 && flag == 1; i += 3)
        if(in[i] != ':' && in[i] != '-') flag = 0;
    return flag;
}

int f_check_separators_format3(char* in)
{
    int i, flag = 1;
    for(i = 4; i <= 10 && flag == 1; i += 5)
        if(in[i] != '.') flag = 0;
    return flag;
}

//0  0  -  5  0  -  F  C  -  A  0  -  6  7  -  2  C
//0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
int f_check_hexa_format1(char* in)
{
    int i = 0, j, flag = 1;
    for(j = 0; j <= 15 && flag == 1; j += 3)
        for(i = j; i < (j + 2) && flag == 1; i++)
            if((in[i] < '0' || in[i] > '9') && (in[i] < 'a' || in[i] > 'f')) flag = 0;
    return flag;
}

int f_check_hexa_format2(char* in)
{
    int i, flag = 1;
    for(i = 0; i < 12 && flag == 1; i++)
        if((in[i] < '0' || in[i] > '9') && (in[i] < 'a' || in[i] > 'f')) flag = 0;
    return flag;
}

//a  a  a  a  .  b  b  b  b  .  c  c  c  c
//0  1  2  3  4  5  6  7  8  9  10 11 12 13

int f_check_hexa_format3(char* in)
{
    int i = 0, j, flag = 1;
    for(j = 0; j <= 10 && flag == 1; j += 5)
        for(i = j; i < (j + 4) && flag == 1; i++)
            if((in[i] < '0' || in[i] > '9') && (in[i] < 'a' || in[i] > 'f')) flag = 0;
    return flag;
}

void f_to_lower_case(char* str)
{
    int len_str, i;
    len_str = f_length(str);
    for(i = 0; i < len_str; i++)
        if(str[i] >= 'A' && str[i] <= 'Z') str[i] = str[i] + 0x20;
}

void f_to_upper_case(char* str)
{
    int len_str, i;
    len_str = f_length(str);
    for(i = 0; i < len_str; i++)
        if(str[i] >= 'a' && str[i] <= 'z') str[i] = str[i] - 0x20;
}



int f_get_oldest_command_index(struct_history* history)
{
    int i, tmp_index, tmp_aging;
    for(i = 0; i < 10; i++)
        if(history[i].aging_time == 0)
            return i;

    tmp_index = 0;
    tmp_aging = 0;
    for(i = 0; i < 10; i++)
        if(history[i].aging_time > tmp_aging)
        {
            tmp_aging = history[i].aging_time;
            tmp_index = i;
        }
    return tmp_index;
}

void f_increase_aging(struct_history* history)
{
    int i;
    for(i = 0; i < 10; i++)
        if(history[i].aging_time > 0) history[i].aging_time++;
}

int f_is_history_loaded(struct_history* history)
{
    int i;
    for(i = 0; i < 10; i++)
        if(history[i].aging_time != 0) return 1;
    return 0;
}

int f_get_index_by_aging(struct_history* history, int aging)
{
    int i;
    for(i = 0; i < 10; i++) if(history[i].aging_time == aging) return i;
    return -1;
}

/*
255.255.255.255/32  len = 18
0.0.0.0/0           len = 9
*/
int f_is_subnet(char* subnet)
{
    int subnet_length = f_length(subnet), i, j, j_1 = 0, j_2 = 0;
    char octet_str[4] = {0};
    int octet_int;
    if(subnet_length > 18 || subnet_length < 9) return 0;
    for(i = 0; i < subnet_length; i++)
    {
        if(subnet[i] != '.' && subnet[i] != '/' && (subnet[i] < '0' || subnet[i] > '9')) return 0;
    }
    for(i = 0; i < subnet_length; i++)
    {
        switch(subnet[i])
        {
            case '.':
                j_1++;
                break;
            case '/':
                if(j_1 < 3) return 0;
                else j_2++;
                break;
            default:
                break;
        }
    }
    if(j_1 != 3 || j_2 != 1) return 0;
    j_1 = 0;
    for(i = 0; i < 4; i++)
    {
        j_2 = 0;
        while(subnet[j_1] != '.' && subnet[j_1] != '/')
        {
            if(j_2 > 2) return 0;
            octet_str[j_2] = subnet[j_1];
            j_1++;
            j_2++;
        }
        j_1++;
        octet_int = atoi(octet_str);
        if(octet_int > 255 || octet_int < 0) return 0;
        for(j = 0; j < 4; j++) octet_str[j] = 0;
    }
    j_2 = 0;
    while(j_1 < subnet_length)
    {
        if(j_2 > 1) return 0;
        octet_str[j_2] = subnet[j_1];
        j_2++;
        j_1++;
    }
    octet_int = atoi(octet_str);
    if(octet_int > 32 || octet_int < 0) return 0;
    return 1;
}

void f_get_subnet_data(char* subnet)
{
  ;
}

/*
 * function that filters key pressed
 */
int f_is_key(int char_hit)
{
  if((char_hit >= '0' && char_hit <= '9')
     ||
     (char_hit >= 'A' && char_hit <= 'Z')
     ||
     (char_hit >= 'a' && char_hit <= 'z')
     ||
     (char_hit == ':')
     ||
     (char_hit == '-')
     ||
     (char_hit == '.')
     ||
     (char_hit == ' ')
     ||
     (char_hit == '/')
    ) return 1;
  return 0;
}

/*
 * checks if argument is string is a number
 */
int is_valid_digit(char *str)
{
  while(*str)
  {
    if(!isdigit(*str)) return 0;
    str++;
  }
  return 1;
}

/*
 * checks if string is an IP in a.b.c.d format
 */
int is_ip(char* ip)
{
  char tmp_ip[D_COMMAND_LEN] = {0};
  int dotCount = 0, i;
  char *token;
  for(i = 0; i < D_COMMAND_LEN && ip[i] != 0; i++) tmp_ip[i] = ip[i];
  token = strtok(tmp_ip, ".");
  while(token != NULL)
  {
    if(!is_valid_digit(token)) return 0;
    int num = atoi(token);
    if(num < 0 || num > 255) return 0;
    dotCount++;
    token = strtok(NULL, ".");
  }
  if(dotCount != 4) return 0;
  return 1;
}

/*
 * checks if string is a number
 */
int isNumber(char* string)
{
    if (string == NULL) return 0;
    for (int i = 0; string[i] != '\0'; i++)
        if (string[i] < '0' || string[i] > '9') return 0;
    return 1;
}

/*
 * converts string to int
 */
int toInt(char* str_port)
{
  if (!isNumber(str_port)) return -1;
  return atoi(str_port);
}

/*
 * checks if string is a TCP/UDP port
 */
int isPort(char* str_port, int* port)
{
  *port = toInt (str_port);
  if (*port == -1)
    {
      *port = 0;
      return 0;
    }
  if (*port > 0 && *port < 65535) return 1;
  *port = 0;
  return 0;
}

/*
 *  functions to check if subnet is valid
 *  the main function to use is isSubnet();
 */
bool isInRange(int num, int min, int max)
{
    return (num >= min && num <= max);
}

bool isValidIPPart(char* part)
{
    int num = atoi(part);
    return isInRange(num, 0, 255);
}

bool isValidIPAddress(char* ip)
{
    char* token;
    char* rest = ip;
    int count = 0;
    while ((token = strtok_r(rest, ".", &rest)))
    {
        if (!isValidIPPart(token)) return false;
        count++;
    }
    return count == 4;
}

bool isValidSubnet(char* subnet)
{
    char* slash = strchr(subnet, '/');
    if (slash == NULL) return false;
    *slash = '\0';
    if (!isValidIPAddress(subnet)) return false;
    int prefix = atoi(slash + 1);
    return isInRange(prefix, 0, 32);
}

int isSubnet(char* string)
{
    char subnet[20] = {};
    strcpy (subnet, string);
    return isValidSubnet(subnet) ? 1 : 0;
}

/*
 * get matrix with all IPs inside a subnet ---------------------------------------------------------
 */
void parseSubnet(char* subnet, char* ip, int* prefix)
{
    char* slash = strchr(subnet, '/');
    *slash = '\0';
    strcpy(ip, subnet);
    *prefix = atoi(slash + 1);
}

void parseIP(char* ip, uint32_t* res)
{
    *res = 0;
    char n[4] = {0};
    for (int octet = 0; *ip != 0; ip++)
      {
        if (*ip == '.')
	  {
	    *res = (*res | atoi (n)) << 8;
	    octet = 0;
	    n[0] = 0;
	    n[1] = 0;
	    n[2] = 0;
	  }
	else
	  {
	    n[octet] = *ip;
	    octet++;
	  } 
      }
    *res = (*res | atoi (n));
}

void cleanSubID(uint32_t* ip, int prefix)
{
  uint32_t a = 0;
  for (int i = 0; i < prefix; i++)
    {
      a = a >> 1;
      a = (a | 0x80000000);
    }
  *ip = *ip & a;
}

char** getIPs(char* subnet, int* ips_len)
{
    char ip[16];
    int prefix;
    uint32_t int_ip = 0;
    uint32_t maxIPs;
    char** ips;

    parseSubnet(subnet, ip, &prefix);
    maxIPs = (1 << (32 - prefix));
    ips = (char**)malloc((maxIPs) * sizeof(char*));

    if (ips == NULL)
    {
        perror("Errore durante l'allocazione di memoria");
        exit(EXIT_FAILURE);
    }
    parseIP (ip, &int_ip);
    cleanSubID (&int_ip, prefix);

    for (uint32_t i = 0; i < maxIPs; i++)
    {
        ips[i] = (char*)calloc(16, sizeof(char));
        if (ips[i] == NULL)
	{
            printf("\nErrore durante l'allocazione di memoria");
            *ips_len = 0;
	    return NULL;
        }
        uint32_t octet1 = ((i >> 24) & 0xFF) | ((int_ip >> 24) & 0xFF);
        uint32_t octet2 = ((i >> 16) & 0xFF) | ((int_ip >> 16) & 0xFF);
        uint32_t octet3 = ((i >> 8) & 0xFF) | ((int_ip >> 8) & 0xFF);
        uint32_t octet4 = (i & 0xFF) | ((int_ip) & 0xFF);
        sprintf(ips[i], "%u.%u.%u.%u", octet1, octet2, octet3, octet4);
    }
    *ips_len = maxIPs;
    return ips;
}
/*
 * -------------------------------------------------------------------------------------------------
 */

//debug functions ----------------------------------------------------------------------------------

void f_print_history(struct_history *history)
{
    int i, x, y;

    getXY(&x, &y);
    gotoXY(3, y+1);
    _cprintf("Aging time\tcommand\n");
    for(i = 0; i < 10; i++)
    {
        _cprintf("   %-11d\t%s", history[i].aging_time, history[i].command);
        if(i < 9) _putch('\n');
    }
}

void f_print_matrix(char **matrix)
{
    int i, x, y;

    getXY(&x, &y);
    gotoXY(3, y+1);
    for(i = 0; i < 10; i++)
    {
        _cprintf("   %s", matrix[i]);
        if(i < 9) _putch('\n');
    }
}
