//
//  FileReader.hpp
//  MapQueue
//
//  Created by Hao WU on 3/12/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#ifndef FileReader_h
#define FileReader_h

#include <stdio.h>
#include <queue>
#include <iostream>
#include "Packet.h"

typedef std::vector<char> Data;
class FileReader{
private:
    std::deque<Data> my_filequeue;
public:
    FileReader();
    long readFile(std::string filename);
    bool isEnd();
    Data getNextData();
};
#endif /* FileReader_hpp */
