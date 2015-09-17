#include <stdlib.h>
#include <string.h>
#include "p33Fxxxx.h"

/* expects 0xdeadbeef or deadbeef */
unsigned int atox(unsigned char *ip_str)
{
    unsigned char tmp[2]={'2','\0'};
    unsigned int op_val=0, i=0, ip_len;
    ip_len = strlen(ip_str) - 1;

    if(strncmp(ip_str, "0x", 2) == 0){
        ip_str +=2;
        ip_len -=2;
    }

    for(i=0;i<ip_len;i++){

        op_val *= 0x10;

        switch(ip_str[i]){
            case 'a':
                op_val += 0xa;
            break;

            case 'b':
                op_val += 0xb;
            break;

            case 'c':
                op_val += 0xc;
            break;

            case 'd':
                op_val += 0xd;
            break;

            case 'e':
                op_val += 0xe;
            break;

            case 'f':
                op_val += 0xf;
            break;

            case '0' ... '9':
                tmp[0] = ip_str[i];
                op_val += atoi(tmp);
            break;

            default :
                op_val += 0x0;
            break;

        }
    }
    
    return op_val;
}