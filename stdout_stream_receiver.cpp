#include "stdout_stream_receiver.h"
#include <iostream>
#include <string>
// Writes the data to stdout
bool StdoutStreamReceiver::receive(const char* data, size_t length){
    std::cout << std::string(data, length) ;
    return true;
}