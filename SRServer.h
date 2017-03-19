//
//  SRServer.hpp
//  Window_Version2
//
//  Created by Hao WU on 3/13/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#ifndef SRServer_h
#define SRServer_h

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "SenderWindow.h"

class SRServer{
private:
    char buffer[BUFFSIZE];
    struct sockaddr_in my_address;
    struct sockaddr_in client_address;
    socklen_t clienet_addrlen;
    int sockid;
    uint16_t init_seq_num;//init_seq_num will be the sequence number in SYNACK
    
    SenderWindow my_SenderWindow;
    
    void send_all_avail_pkt(bool start);// when start is true, it is used for the sending the initial packet
    bool receiveACK(uint16_t &seq_num);//receive ACK and return bool
    
    void resend_all();
    std::unordered_map<uint16_t, clock_t> timestamp;
    void resend(uint16_t seq);
    uint16_t first_ACK_seq;
public:
    SRServer(int port);
    
    std::string HandShake();// will receive SYN, send SYNACK, receive ACK1. and return filename
    void PrepareFile(std::string filename);
    void sendFile();
    void sendFIN();
    void receiveFINACK();
    
    void tearDown();
};
#endif /* SRServer_hpp */
