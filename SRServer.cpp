//
//  SRServer.cpp
//  Window_Version2
//
//  Created by Hao WU on 3/13/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "SRServer.h"
#include <fstream>
#include <thread>
#include <chrono>
SRServer::SRServer(int port) : my_SenderWindow(){
    int sockid_=socket(AF_INET, SOCK_DGRAM, 0);// return the file descriptor of the new socket
    if (sockid_==-1) {
        perror("Socket Creation Failed");
        exit(1);
    }
    SRServer::sockid=sockid_;
    memset((char*) &(this->my_address), 0, sizeof(this->my_address));
    my_address.sin_family=AF_INET;
    my_address.sin_port=htons(port);
    my_address.sin_addr.s_addr=htonl(INADDR_ANY);
    
    if (bind(sockid, (struct sockaddr*) &my_address, sizeof(my_address))==-1) {// bind socket to add port
        perror("Bind Failed");
        exit(1);
    }
    SRServer::clienet_addrlen=sizeof(client_address);
}

std::string SRServer::HandShake(){
    ssize_t receive_len=-1;
    std::srand (unsigned(time(0)));
    init_seq_num=rand()%MAXSEQ;
    std::string request_file_name="";
    int retransmit_count=0;
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len<0) {
            perror("Receive Packet SYN/ACK1 error\n");
            exit(EXIT_FAILURE);
        }
        Data mydata(buffer,buffer+receive_len);
        Packet recv_pkt(mydata);
        //std::cout<<"receive a pkt"<<std::endl;
        //std::cout<<"Packet ACK status "<<recv_pkt.isACK()<<std::endl;
        //std::cout<<"Packet ACK number "<<recv_pkt.getAckNumber()<<std::endl;
        
        std::cout<<"Receiving Packet "<<recv_pkt.getAckNumber()<<std::endl;
        if (recv_pkt.isSYN()) {// Generate the first Sequence Number Once receive the SYN
            
            uint16_t my_ack_num=recv_pkt.getSeq()+1;
            Packet mySYNACK;
            mySYNACK.setSYN(true);
            mySYNACK.setACK(true);
            mySYNACK.setAckNumber(my_ack_num);
            mySYNACK.setSeq(init_seq_num);
            Data loaded_SYNACK=mySYNACK.to_UDPData();
            
            //std::fstream outfile2;
            //outfile2.open("send.data", std::ios::app|std::ios::out|std::ios::binary);
            //outfile2.write(loaded_SYNACK.data(), loaded_SYNACK.size());
            while (true) {
                if (sendto(sockid, loaded_SYNACK.data(), loaded_SYNACK.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
                    perror("SYNACK sent failed\n");
                    continue;
                }
                else{
                    if (retransmit_count==0) {
                        std::cout<<"Sending packet "<<mySYNACK.getSeq()<<" 5120 SYN"<<std::endl;
                        retransmit_count++;
                    }
                    else{
                        std::cout<<"Sending packet "<<mySYNACK.getSeq()<<" 5120 Retransmission SYN"<<std::endl;
                    }
                    //std::cout<<"SYNACK sent! My seq number is "<<init_seq_num<<std::endl;
                    //std::cout<<"My seq number is "<<mySYNACK.getSeq()<<std::endl;
                    break;
                }
            }
            continue;
        }
        if (recv_pkt.isACK() && recv_pkt.getAckNumber()==init_seq_num) {
            for (int i=0; i<recv_pkt.getData().size(); i++) {
                request_file_name.push_back(char(recv_pkt.getData()[i]));
            }
            break;
        }
    }
    //std::cout<<"Handshake Complete!"<<std::endl;
    return  request_file_name;
}

void SRServer::PrepareFile(std::string filename){// The sequence number add one
    if (filename=="") {
        return;
    }
    my_SenderWindow.prepareFile(filename,init_seq_num+1);
}

void SRServer::send_all_avail_pkt(bool start){
    while (my_SenderWindow.is_next_ready_to_send()) {
        SenderWindow::Sender_Combo ready_to_send_combo=my_SenderWindow.retreive_nextCombo_tosend();
        Packet ready_to_send_packet;
        ready_to_send_packet.loadData(ready_to_send_combo.data);
        ready_to_send_packet.setSeq(ready_to_send_combo.seq_num);
        if (start) {
            first_ACK_seq=ready_to_send_combo.seq_num;
            start=false;
            ready_to_send_packet.setACK(true);
        }
        Data loaded_data=ready_to_send_packet.to_UDPData();
        while (true) {
            if (sendto(sockid, loaded_data.data(), loaded_data.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
                perror("File sent failed\n");
                continue;
            }
            else{
                std::cout<<"Sending packet "<<ready_to_send_combo.seq_num<<" 5120 "<<std::endl;
                clock_t cur_time=clock();
                timestamp.insert(std::make_pair(ready_to_send_combo.seq_num, cur_time));
                break;
            }
        }
    }
}

bool SRServer::receiveACK(uint16_t &seq_num){
    ssize_t receive_len=-1;
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, MSG_DONTWAIT, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len<0){
            return false;
        }
        Data mydata(buffer,buffer+receive_len);
        Packet recv_pkt(mydata);
        
        std::cout<<"Receiving Packet "<<recv_pkt.getAckNumber()<<std::endl;
        if (recv_pkt.isACK()) {
            //std::cout<<"ACK received!"<<std::endl;
            if (timestamp.find(recv_pkt.getAckNumber
                               
                               ())!=timestamp.end()) {
                timestamp.erase(recv_pkt.getAckNumber());
                //std::cout<<"ACK status: for retransmitted one"<<std::endl;
            }
            seq_num=recv_pkt.getAckNumber();
            return true;
        }
        else{
            perror("ACK reception failure!\n");
            continue;
        }
    }
}

void SRServer::sendFile(){
    send_all_avail_pkt(true);
    bool receive_any=false;
    uint16_t received_ACK_num=0;
    while (!my_SenderWindow.empty()) {
        //uint16_t received_ACK_num=receiveACK();// IO block
        receive_any=receiveACK(received_ACK_num);
        if (receive_any) {
            my_SenderWindow.receive_ACK(received_ACK_num);
        }
        resend_all();
        send_all_avail_pkt(false);
    }
}

void SRServer::resend_all(){
    for (auto it=timestamp.begin(); it!=timestamp.end(); it++) {
        double duration = (clock() - it->second) / (double) CLOCKS_PER_SEC;
        if (duration>0.5) {
            //std::cout<<"resending "<<it->first<<std::endl;
            resend(it->first);
        }
    }
}

void SRServer::resend(uint16_t seq){
    SenderWindow::Sender_Combo ready_to_send_combo=my_SenderWindow.retreive_Combo_byID(seq);
    Packet ready_to_send_packet;
    ready_to_send_packet.loadData(ready_to_send_combo.data);
    ready_to_send_packet.setSeq(ready_to_send_combo.seq_num);
    if (seq==first_ACK_seq) {
        ready_to_send_packet.setACK(true);
    }
    Data loaded_data=ready_to_send_packet.to_UDPData();
    while (true) {
        if (sendto(sockid, loaded_data.data(), loaded_data.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("File sent failed\n");
            continue;
        }
        else{
            std::cout<<"Sending packet "<<ready_to_send_combo.seq_num<<" 5120 Retransmission"<<std::endl;
            clock_t cur_time=clock();
            timestamp[ready_to_send_combo.seq_num]=cur_time;
            //std::thread(resend_pkt, ready_to_send_combo.seq_num,std::ref(*this)).detach();
            break;
        }
    }
}

void SRServer::sendFIN(){
    Packet myFIN;
    myFIN.setFIN(true);
    Data loaded_Fin=myFIN.to_UDPData();
    while (true) {
        if (sendto(sockid, loaded_Fin.data(), loaded_Fin.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("FIN sent failed\n");
            continue;
        }
        else{
            std::cout<<"FIN sent!"<<std::endl;
            break;
        }
    }
    return;
}

void SRServer::receiveFINACK(){
    ssize_t receive_len=-1;
    while (true) {
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, 0, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len<0) {
            perror("Receive FINACK error\n");
            exit(1);
        }
        Data myData(buffer,buffer+receive_len);
        Packet recv_pkt(myData);
        if (recv_pkt.isFIN() && recv_pkt.isACK()) {
            std::cout<<"Receiving Packet "<<recv_pkt.getAckNumber()<<std::endl;
            break;
        }
        else{
            std::cout<<"Receiving Packet "<<recv_pkt.getAckNumber()<<std::endl;
            //perror("FINACK packet garbled\n");
            continue;
        }
    }
    close(this->sockid);
}

void SRServer::tearDown(){
    Packet myFIN;
    myFIN.setFIN(true);
    Data loaded_Fin=myFIN.to_UDPData();
    ssize_t receive_len=-1;
    int resend_count=0;
    while (true) {
        if (sendto(sockid, loaded_Fin.data(), loaded_Fin.size(), 0, (struct sockaddr *) &client_address , clienet_addrlen)<0) {
            perror("FIN sent failed\n");
            continue;
        }
        else{
            if (resend_count==0) {
                std::cout<<"Sending packet "<<myFIN.getSeq()<<" 5120 FIN"<<std::endl;
                resend_count++;
            }
            else{
                std::cout<<"Sending packet "<<myFIN.getSeq()<<" 5120 Retransmission FIN"<<std::endl;
            }
            //std::cout<<"FIN sent!"<<std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        memset(buffer, 0, BUFFSIZE);
        receive_len=recvfrom(this->sockid, buffer, BUFFSIZE, MSG_DONTWAIT, (struct sockaddr*) &client_address, &clienet_addrlen);
        if (receive_len>0) {
            Data myData(buffer,buffer+receive_len);
            Packet recv_pkt(myData);
            
            std::cout<<"Receiving Packet "<<recv_pkt.getAckNumber()<<std::endl;
            if (recv_pkt.isFIN() && recv_pkt.isACK()) {
                //printf("FINACK received!\n");
                break;
            }
            else{
                //perror("FINACK packet garbled\n");
                continue;
            }
        }
    
    }
    close(this->sockid);
}



