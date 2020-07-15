#include "multiaddr/protocols.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "utils/varhexutils.h"

/*
int protocol_REMOVE_id(struct ProtocolListItem* protocol_P, int remid)//Function
to remove & shift back all data, sort of like c++ vectors.
{

        if(remid < CNT_PROTOCOLNUM && remid >= 0&&CNT_PROTOCOLNUM!=0) //Checking
to see if remid actually exists.
        {
                for(int i=remid; i<CNT_PROTOCOLNUM-1; ++i) //While i < num of
registered protocols //Needs to be tested that -1 is for valgrind debugging
                {
                        //strcpy((protocol_P+i)->hexcode,
(protocol_P+i+1)->hexcode); //shift memory to the user we want to remove.
                        (protocol_P+i)->deccode = (protocol_P+i+1)->deccode;
//Same as above (protocol_P+i)->size = (protocol_P+i+1)->size;
//Same as above strcpy((protocol_P+i)->name, (protocol_P+i+1)->name);
//Same as above
                }//Overwriting user done. Time to get rid of that extra memory.
                protocol_P = (struct Protocol*) realloc(protocol_P,
(CNT_PROTOCOLNUM-1) * sizeof(struct Protocol));
                //Memory erased,
                CNT_PROTOCOLNUM--; //Since the record no longer exists, we
should decrease the ammount of users. return 1; //Purely for error checking, in
case someone ever wants it/ }	//1 = Success else
        {
                if(CNT_PROTOCOLNUM == 0)
                {
                        perror("ERROR: 0 PROTOCOLS... Did you load protocols?");
                }
                else
                {
                        perror("ERROR: No such protocol!");
                }
                return 0;
        }
}
*/

void unload_protocols(struct ProtocolListItem *head) {
    struct ProtocolListItem *current = head;
    while (current != NULL) {
        struct ProtocolListItem *next = current->next;
        free(current->current);
        free(current);
        current = next;
    }
}

/**
 * load the available protocols into the global protocol_P
 * @returns True(1) on success, otherwise 0
 */
int load_protocols(struct ProtocolListItem **head) {
    unload_protocols(*head);
    int num_protocols = 14;
    int dec_code[] = {4, 41, 6, 17, 33, 132, 301, 302, 42, 480, 443, 477, 444, 275};
    int size[] = {32, 128, 16, 16, 16, 16, 0, 0, -1, 0, 0, 0, 10, 0};
    char *name[] = {
        "ip4", "ip6",  "tcp",  "udp",   "dccp", "sctp",  "udt",
        "utp", "ipfs", "http", "https", "ws",   "onion", "libp2p-webrtc-star"};
    struct ProtocolListItem *last = NULL;
    for (int i = 0; i < num_protocols; i++) {
        struct ProtocolListItem *current_item =
            (struct ProtocolListItem *)malloc(sizeof(struct ProtocolListItem));
        if (current_item == NULL) {
            return 0;
        }
        current_item->current = (struct Protocol *)malloc(sizeof(struct Protocol));
        if (current_item->current == NULL) {
            return 0;
        }
        current_item->next = NULL;
        current_item->current->deccode = dec_code[i];
        strcpy(current_item->current->name, name[i]);
        current_item->current->size = size[i];
        if (*head == NULL) {
            *head = current_item;
        } else {
            last->next = current_item;
        }
        last = current_item;
    }
    return 1;
}

/*
void load_protocols_from_file(struct Protocol** in)
{
        struct Protocol* protocol_P = *in;
        FILE *FPROC_POINT; //File pointer.
        FPROC_POINT = fopen("proto-dat", "r");//Opening proto-dat Or
protocols.csv, I just formatted it to my liking. if(FPROC_POINT != NULL) //While
pointer is not null.
        {
                char W_BUFF[20] = "\0";//Char array to parse file.
                for(int i=0; fscanf(FPROC_POINT, "%s", W_BUFF) != EOF; i++) //
Scanning file and incrementing for processing.
                {
                        switch(i)
                        {
                                case 0: //First word - HEXCODE
                                {
                                        //ADD MEMORY FOR NEW PROTOCOL
                                        if(CNT_PROTOCOLNUM==0) //If there are no
registered protocols yet, allocate memory to pointer.
                                        {
                                                protocol_P = (struct Protocol*)
malloc (sizeof(struct Protocol));
                                        }
                                        else //Reallocate memory to fit one more
Protocol
                                        {
                                                protocol_P = (struct Protocol*)
realloc(protocol_P, (CNT_PROTOCOLNUM+1) * sizeof(struct protocol));
                                        }
                                        //strcpy((protocol_P+CNT_PROTOCOLNUM)->hexcode,
W_BUFF);	//Copy word to structure at hexcode A hexcode is a string so we
keep it as such break;
                                }
                                case 1://Second word - DECCODE
                                {
                                        (protocol_P+CNT_PROTOCOLNUM)->deccode=
atoi(W_BUFF);	//Copy word to structure at deccode after converting it to int.
                                        break;
                                }
                                case 2://Third word - SIZE
                                {
                                        (protocol_P+CNT_PROTOCOLNUM)->size=
atoi(W_BUFF);		//Copy word to structure at size after converting it to
int. break;
                                }
                                case 3://Fourth word - NAME
                                {
                                        strcpy((protocol_P+CNT_PROTOCOLNUM)->name,
W_BUFF); 	//Copy word to structure at name // String i=-1;
                                        CNT_PROTOCOLNUM++;
                                        break;
                                }
                                default:
                                {
                                        printf("HOUSTON WE HAVE A BIG
PROBLEM!!!!\nPROTOCOLS.H-REACHED DEFAULT CASE IN READING FILE!\nREPORT TO
SYSTEMS ADMIN!\n"); break;
                                }
                        }
                }
                fclose(FPROC_POINT);
                protocol_REMOVE_id(protocol_P, 0);
        }
        else
        {
                perror("Fatal Error:");
        }
}
*/

struct Protocol *
proto_with_name(const struct ProtocolListItem *head,
                const char *proto_w_name) // Search for Protocol with inputted name
{
    const struct ProtocolListItem *current = head;
    while (current != NULL) {
        if (strcmp(proto_w_name, current->current->name) == 0) {
            return current->current;
        }
        current = current->next;
    }
    return NULL;
}

struct Protocol *
proto_with_deccode(const struct ProtocolListItem *head,
                   int proto_w_deccode) // Search for Protocol with inputted deccode
{
    const struct ProtocolListItem *current = head;
    while (current != NULL) {
        if (current->current->deccode == proto_w_deccode) {
            return current->current;
        }
        current = current->next;
    }
    return NULL;
}

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
void protocols_with_string(const struct ProtocolListItem *head, char *meee,
                           int sizi) // NOT FINISHED, DO NOT USE!
{
    int finalsize = 0;

    if (!isalnum(meee[sizi - 1]) && !isalnum(meee[sizi - 1])) {
        // Everything is alright, it's nul terminated!;
        finalsize = sizi;
    } else {
        // Well houston we have a problem.
        finalsize = sizi + 2;
    }
    char mestring[finalsize];
    strcpy(mestring, meee);
    if (sizi != finalsize) {
        strcpy(mestring, "\0");
    }

    char *words[50] = {NULL};
    int atword = 0;
    int mem = 0;
    for (int i = 0; i < (int)(sizeof(mestring) - 2); i++) {
        if (mestring[i] == '/') {
            printf("NEW WORD!\n");
            atword++;
            int currentsize = 0;
            for (int j = i + 1;
                 mestring[j] != '/' && j < (int)(sizeof(mestring) - 2); j++) {
                currentsize++;
            }
            char haay[20];
            int lesbo = 0;
            for (int x = i + 1; x < (int)(sizeof(mestring) - 2); x++) {
                if (mestring[x] == '/') {
                    break;
                }
                haay[lesbo] = mestring[x];
                lesbo++;
            }
            words[atword - 1] = (char *)malloc(currentsize + 2);
            // analyzer-possible-null-argument
            if (words[atword - 1] == NULL) {
                break;
            }
            strcpy(words[atword - 1], haay);
            memset(haay, 0, 20);
            /*!
             * @todo TODO(bonedaddy): figure out why bzero is not
             * present and if we should use
             */
            // bzero(haay,20);
        }
    }
    printf("Result:%s\n", words[0]);
    for (int mm = 0; mm < 50; mm++) {
        if (words[mm]) {
            free(words[mm]);
        }
    }
}
