//
//  MapQueue.hpp
//  Window_Version2
//
//  Created by Hao WU on 3/13/17.
//  Copyright © 2017 Hao WU. All rights reserved.
//

#ifndef MapQueue_h
#define MapQueue_h
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <deque>
template <class Combo>
class UniqueMapqueue2{// The key will be the seq_num
private:
    std::unordered_map<uint16_t,Combo> my_innermap;
    std::deque<uint16_t> my_innerqueue;
public:
    UniqueMapqueue2();
    uint16_t front_key();
    uint16_t back_key();
    Combo front_value();
    Combo back_value();
    void pop_back();
    void pop_front();
    void push_back(std::pair<uint16_t,Combo> new_pair);
    void push_front(std::pair<uint16_t,Combo> new_pair);
    Combo& operator[](uint16_t key);// overload should use reference, [] does not use referrence
    bool empty();
    unsigned long size();
    bool haskey(uint16_t key);
    Combo getValueByIndex(int &i);
};

template<class Combo>
UniqueMapqueue2<Combo>::UniqueMapqueue2() : my_innerqueue(), my_innermap(){
}

template<class Combo>
uint16_t UniqueMapqueue2<Combo>::back_key(){
    return my_innerqueue.back();
}

template<class Combo>
uint16_t UniqueMapqueue2<Combo>::front_key(){
    return my_innerqueue.front();
}

template<class Combo>
Combo UniqueMapqueue2<Combo>::back_value(){
    uint16_t backkey=back_key();
    return my_innermap[backkey];
}

template<class Combo>
Combo UniqueMapqueue2<Combo>::front_value(){
    uint16_t frontkey=front_key();
    return my_innermap[frontkey];
}

template<class Combo>
void UniqueMapqueue2<Combo>::pop_back(){
    if (size()==0) {
        perror("pop_back: empty mapqueue!\n");
        exit(EXIT_FAILURE);
    }
    uint16_t backkey=back_key();
    my_innerqueue.pop_back();
    my_innermap.erase(backkey);
}

template<class Combo>
void UniqueMapqueue2<Combo>::pop_front(){
    if (size()==0) {
        perror("pop_front: empty mapqueue!\n");
        exit(EXIT_FAILURE);
    }
    uint16_t frontkey=front_key();
    my_innerqueue.pop_front();
    my_innermap.erase(frontkey);
}

template<class Combo>
void UniqueMapqueue2<Combo>::push_front(std::pair<uint16_t, Combo> new_pair){
    uint16_t frontkey=new_pair.first;
    std::string frontvalue=new_pair.second;
    if (my_innermap.find(frontkey)!=my_innermap.end()) {
        perror("push_front: Key Collision in MapQueue");
        exit(EXIT_FAILURE);
    }
    my_innermap[frontkey]=frontvalue;
    my_innerqueue.push_front(frontkey);
}

template<class Combo>
void UniqueMapqueue2<Combo>::push_back(std::pair<uint16_t, Combo> new_pair){
    uint16_t backkey=new_pair.first;
    Combo backvalue=new_pair.second;
    if (my_innermap.find(backkey)!=my_innermap.end()) {
        perror("push_back: Key Collision in MapQueue");
        exit(EXIT_FAILURE);
    }
    my_innermap[backkey]=backvalue;
    my_innerqueue.push_back(backkey);
}

template<class Combo>
Combo& UniqueMapqueue2<Combo>::operator[](uint16_t key) {
    if (my_innermap.find(key)!=my_innermap.end()) {
        return my_innermap[key];
    }
    else{
        perror("No such key");
        exit(EXIT_FAILURE);
    }
}

template<class Combo>
bool UniqueMapqueue2<Combo>::haskey(uint16_t key){
    return (my_innermap.find(key)!=my_innermap.end());
}

template<class Combo>
bool UniqueMapqueue2<Combo>::empty(){
    if (my_innermap.empty()!=my_innerqueue.empty()) {
        perror("Empty(): Inner Queue and Inner Map Does Not Match!");
        exit(EXIT_FAILURE);
    }
    return my_innermap.empty();
}

template<class Combo>
unsigned long UniqueMapqueue2<Combo>::size(){
    if (my_innermap.size()!=my_innerqueue.size()) {
        perror("Size(): Inner Queue and Inner Map Does Not Match!");
        exit(EXIT_FAILURE);
    }
    return my_innermap.size();
}

template<class Combo>
Combo UniqueMapqueue2<Combo>::getValueByIndex(int &i){
    uint16_t key=my_innerqueue[i];
    //std::cout<<"key is "<<key<<std::endl;
    if (my_innermap.find(key)==my_innermap.end()) {
        perror("Index Out Of Range");
        exit(EXIT_FAILURE);
    }
    return my_innermap[key];
}

#endif /* MapQueue_hpp */
