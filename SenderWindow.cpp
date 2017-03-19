//
//  SenderWindow.cpp
//  MapQueue
//
//  Created by Hao WU on 3/10/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "SenderWindow.h"
#include <fstream>
#include <algorithm>
#include <ctime>
SenderWindow::SenderWindow() : Window(),myFileReader_r(),WindowSizeLimit(5){
    std::srand (unsigned(std::time(0)));
    //new_data_seq=rand()%MAXSEQ;
    //new_data_seq = 23038;
    //std::cout<<"WARNING: SenderWindow created with random number....CHECK THE CODE!"<<std::endl;
    next_data_index=0;
}

SenderWindow::SenderWindow(uint16_t init_seq) :Window(),myFileReader_r(),WindowSizeLimit(5),new_data_seq(init_seq){
    next_data_index=0;
}

bool SenderWindow::is_next_ready_to_send(){
    return next_data_index<Window.size() && !empty();
}

bool SenderWindow::empty(){
    return Window.size()==0;
}

SenderWindow::Sender_Combo SenderWindow::retreive_nextCombo_tosend(){
    if (next_data_index>Window.size()) {
        perror("No available nextCombo\n");
        exit(EXIT_FAILURE);
    }
    SenderWindow::Sender_Combo res=Window.getValueByIndex(next_data_index);
    next_data_index++;
    return res;
}

void SenderWindow::receive_ACK(uint16_t seq_num){
    if (Window.haskey(seq_num)) {
        //std::cout<<"ACKed "<<seq_num<<std::endl;
        Window[seq_num].ACK=true;
    }
    else{
        //std::cout<<"No ACK: "<<seq_num<<std::endl;
        return;
    }
    while (!Window.empty() && Window.front_value().ACK==true) {
        Window.pop_front();
        next_data_index--;
    }
    //int cur_seq=Window.back_value().seq_num+1;
    long data_count=Window.size();
    while (data_count<WindowSizeLimit && !myFileReader_r.isEnd()) {
        SenderWindow::Sender_Combo read_Combo;
        read_Combo.ACK=false;
        read_Combo.data=myFileReader_r.getNextData();
        
        //read_Combo.seq_num=cur_seq;
        
        read_Combo.seq_num=new_data_seq;
        new_data_seq=(new_data_seq+read_Combo.data.size())%MAXSEQ;
        Window.push_back(std::make_pair(read_Combo.seq_num, read_Combo));
        
        //cur_seq+=read_Combo.data.size();
        data_count++;
    }
}

void SenderWindow::debug_check_window_status(){
    for (int i=0; i<Window.size(); i++) {
        std::cout<<i<<" element, seq: "<<Window.getValueByIndex(i).seq_num<<" ACK: "<<Window.getValueByIndex(i).ACK<<" content: ";//Window.getValueByIndex(i).data<<std::endl;
    }
}

void SenderWindow::prepareFile(std::string filename,uint16_t first_file_pkt_seq){
    long file_size=myFileReader_r.readFile(filename);
    if (file_size<0) {
        return;
    }
    unsigned long data_count=Window.size();
    if (first_file_pkt_seq==MAXSEQ+1) {
        std::srand (unsigned(std::time(0)));
        new_data_seq=rand()%MAXSEQ;
        std::cout<<"WARNING thrown by Hao Wu "<<std::endl;
    }
    else{
        new_data_seq=first_file_pkt_seq;
    }
    while (data_count<WindowSizeLimit && !myFileReader_r.isEnd()) {
        SenderWindow::Sender_Combo read_Combo;
        read_Combo.ACK=false;
        read_Combo.data=myFileReader_r.getNextData();
        
        read_Combo.seq_num=new_data_seq;
        new_data_seq=(new_data_seq+read_Combo.data.size())%MAXSEQ;
        
        Window.push_back(std::make_pair(read_Combo.seq_num, read_Combo));
        data_count++;
    }
    //std::cout<<"reading finished"<<std::endl;
}

SenderWindow::Sender_Combo SenderWindow::retreive_Combo_byID(uint16_t seq){
    if (Window.haskey(seq)) {
        return Window[seq];
    }
    else{
        perror("Sender_Window has no such seq\n");
        exit(EXIT_FAILURE);
    }
}

/*
 void SenderWindow::prepareFile(std::string filename){
 myFileReader_r.readFile(filename);
 int data_count=Window.size();
 while (data_count<WindowSizeLimit && !myFileReader_r.isEnd()) {
 SenderWindow::Sender_Combo read_Combo;
 read_Combo.ACK=false;
 read_Combo.data=myFileReader_r.getNextData();
 
 read_Combo.seq_num=new_data_seq;
 new_data_seq=(new_data_seq+read_Combo.data.size())%MAXSEQ;
 
 Window.push_back(std::make_pair(read_Combo.seq_num, read_Combo));
 data_count++;
 }
 std::cout<<"reading finished"<<std::endl;
 }
*/

void SenderWindow::debug_ACK_all(){// will the ACK all the packet one by one
    std::ofstream outfile("w.bin",std::ios::out | std::ios::app | std::ios::binary);
    //std::cout<<Window.size()<<std::endl;
    while (!this->empty()) {
        uint16_t front_seq=Window.front_key();
        std::cout<<"hh"<<std::endl;
        Sender_Combo front_combo=retreive_nextCombo_tosend();
        std::cout<<"hh1"<<std::endl;
        outfile.write(front_combo.data.data(), front_combo.data.size());
        std::cout<<"hh2"<<std::endl;
        receive_ACK(front_seq);
        std::cout<<"hh3"<<std::endl;
    }
    outfile.close();
}

void SenderWindow::debug_ACK_random(){
    std::ofstream outfile("copy.pdf",std::ios::out | std::ios::app | std::ios::binary);
    while (!empty()) {
        std::cout<<std::endl;
        std::vector<uint16_t> seq_vec;
        for (int i=0; i<Window.size(); i++) {
            seq_vec.push_back(Window.getValueByIndex(i).seq_num);
        }
        std::cout<<"window seq num colection complete"<<std::endl;
        if (is_next_ready_to_send()) {
            Sender_Combo front_combo=retreive_nextCombo_tosend();
            outfile.write(front_combo.data.data(), front_combo.data.size());
            std::cout<<"Sent out the first data"<<std::endl;
        }
        
        // randomly ACK a package
        
        std::srand ( unsigned ( std::time(0) ) );
        std::random_shuffle(seq_vec.begin(), seq_vec.end());
        
        receive_ACK(seq_vec[0]);
        std::cout<<"ACK a package "<<seq_vec[0]<<std::endl;
    }
}
