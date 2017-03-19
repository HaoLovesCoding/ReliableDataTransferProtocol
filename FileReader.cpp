//
//  FileReader.cpp
//  MapQueue
//
//  Created by Hao WU on 3/12/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include "FileReader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
FileReader::FileReader() : my_filequeue(){
}

bool FileReader::isEnd(){
    return my_filequeue.empty();
}

Data FileReader::getNextData(){
    if (my_filequeue.empty()) {
        perror("File read to end\n");
        exit(EXIT_FAILURE);
    }
    Data to_window_data=my_filequeue.front();
    my_filequeue.pop_front();
    return to_window_data;
}

long FileReader::readFile(std::string filename){// 7 in this file is the header length
    std::streampos size;
    char *memblock;
    std::ifstream file(filename,std::ios::in|std::ios::ate|std::ios::binary);
    //std::ofstream outfile("w.bin",std::ios::out | std::ios::app | std::ios::binary);
    //std::ofstream outfile2("w2.bin",std::ios::out | std::ios::app | std::ios::binary);
    if (file.is_open()) {
        size=file.tellg();
        memblock=new char[size];
        file.seekg(0,std::ios::beg);
        file.read(memblock,size);
        //outfile2.write(memblock, size);
        file.close();
        int i=0;
        while (i+BUFFSIZE-7<size) {
            Data data_peice(memblock+i,memblock+i+BUFFSIZE-7);
            //outfile.write(data_peice.data(), data_peice.size());
            //std::cout<<"Data size is"<<data_peice.size()<<std::endl;
            my_filequeue.push_back(data_peice);
            i+=BUFFSIZE-7;
        }
        Data data_peice(memblock+i,memblock+size);
        //outfile.write(data_peice.data(), data_peice.size());
        my_filequeue.push_back(data_peice);
        delete memblock;
    }
    else{
        return -1;
    }
    return (long)size;
}

/*long FileReader::readFile(std::string filename){// 7 in this file is the header length
    std::streampos size;
    char *memblock;
    std::ifstream file(filename,std::ios::in|std::ios::ate|std::ios::binary);
    //std::ofstream outfile("w.bin",std::ios::out | std::ios::app | std::ios::binary);
    //std::ofstream outfile2("w2.bin",std::ios::out | std::ios::app | std::ios::binary);
    if (file.is_open()) {
        size=file.tellg();
        memblock=new char[size];
        file.seekg(0,std::ios::beg);
        file.read(memblock,size);
        //outfile2.write(memblock, size);
        file.close();
        int i=0;
        while (i+BUFFSIZE-7<size) {
            Data data_peice(memblock+i,memblock+i+BUFFSIZE-7);
            //outfile.write(data_peice.data(), data_peice.size());
            //std::cout<<"Data size is"<<data_peice.size()<<std::endl;
            my_filequeue.push_back(data_peice);
            i+=BUFFSIZE-7;
        }
        Data data_peice(memblock+i,memblock+size);
        //outfile.write(data_peice.data(), data_peice.size());
        my_filequeue.push_back(data_peice);
        delete memblock;
    }
    else{
        perror("File open error\n");
        exit(EXIT_FAILURE);
    }
    return (long)size;
}*/
