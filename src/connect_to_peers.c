
#include "util.h"
#include "btdata.h"

void *connect_to_peer(void *p){
    int k = (int)p;
    peer_t *mypeer = &peers_pool[k];
    pthread_mutex_lock(&mypeer->sock_mutex);
    if(mypeer->sockfd < 0){
        mypeer->sockfd = connect_to_host(mypeer->ip, mypeer->port);
        pthread_t thread;
        int rc = pthread_create(&thread, NULL, recv_from_peer, (void *)k);
        if(rc){
            printf("ERROR, return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_mutex_unlock(&mypeer->sock_mutex);
    char *shkhdmsg;
    char *current;
    int msglen = 0;

    shkhdmsg = (char*)malloc(HANDSHAKE_LEN * sizeof(char));
    current = shkhdmsg;

    char pstrlen = (char)strlen(BT_PROTOCOL);
    *current++ = pstrlen;
    strncpy(current, BT_PROTOCOL, pstrlen);
    current += pstrlen;

    memset(current, 0, 8);
    current += 8;

    int i = 0;
    for(; i < 5; i ++){
        int j = 0;
        int part = reverse_byte_orderi(g_infohash[i]);
        unsigned char *p = (unsigned char*)&part;
        for(; j < 4; j ++){
            *current++ = p[j];
        }
    }
    
    for(i = 0; i < 20; i ++){
        current += sprintf(current, "%02x", (unsigned char)g_my_id[i]);
    }

    msglen = current - shkhdmsg;

    if(mypeer->status == 0){
        send(mypeer->sockfd, shkhdmsg, msglen, 0);
    } 
    free(shkhdmsg);

}
