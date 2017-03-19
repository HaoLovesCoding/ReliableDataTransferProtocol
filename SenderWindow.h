//
//  SenderWindow.hpp
//  MapQueue
//
//  Created by Hao WU on 3/10/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#ifndef SenderWindow_h
#define SenderWindow_h

#include <stdio.h>
#include <iostream>
#include "MapQueue.h"
//#include "FileReadingSimulator.h"
#include "FileReader.h"
#include "Packet.h"
class SenderWindow{
public:
    struct Sender_Combo{
        bool ACK;
        uint16_t seq_num;
        Data data;
    };
    SenderWindow();
    SenderWindow(uint16_t init_seq);
    SenderWindow::Sender_Combo retreive_nextCombo_tosend();
    bool is_next_ready_to_send();
    bool empty();
    void receive_ACK(uint16_t seq_num);
    void prepareFile(std::string filename, uint16_t first_file_pkt_seq=MAXSEQ+1);
    
    void debug_check_window_status();
    void debug_ACK_all();
    void debug_ACK_random();
    
    SenderWindow::Sender_Combo retreive_Combo_byID(uint16_t seq);
private:
    UniqueMapqueue2<Sender_Combo> Window;
    //FileReadingSimulator myFileReader;
    FileReader myFileReader_r;
    const int WindowSizeLimit;
    int next_data_index;
    uint16_t new_data_seq;
};




#endif /* SenderWindow_hpp */
