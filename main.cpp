//
//  main.cpp
//  Window_Version2
//
//  Created by Hao WU on 3/13/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#include <iostream>
#include "SRServer.h"
int main(int argc, const char * argv[]) {
    if (argc!=2) {
        std::cout<<"Usage: server + Port_number\n";
        exit(1);
    }
    int port=atoi(argv[1]);
    SRServer mySRServer(port);
    std::string testfile=mySRServer.HandShake();
    mySRServer.PrepareFile(testfile);
    mySRServer.sendFile();
    mySRServer.tearDown();
    return 0;
}
