
#include <iostream>
#include <sys/epoll.h>
#include <cstring>
#include "version.h"
#include "message_merge.h"
#include "inbound_pipe.h"
#include "../serial_port.h"
#include "inbound_pipe_epoll_adapter.h"
#include "serial_epoll_adapter.h"



MessageMerge merge;

int main(int argc, char* argv[]){
    std::cout << "Flarm Merge Version " << VERSION_MAJOR << '.' << VERSION_MINOR << std::endl;

    const char* fifo = "/opt/sky_echo/sky_echo_fifo";
    const char* serialDevice = "/dev/serial0";
    int baud = 38400;

    InboundPipe pipe(fifo);
    SerialPort serial(serialDevice, baud);

    // See https://www.man7.org/linux/man-pages/man7/epoll.7.html
    int pollFd = epoll_create(4);  // up to 4 file descriptors but doesn't actually matter!
    if(pollFd == -1){
        std::cerr << "Unable to create EPOLL " << strerror(errno) << std::endl;
        return 1;
    }

    InboundPipeEpollAdapter pipeAdapter(&merge, &pipe);
    struct epoll_event event;
    event.data.ptr = &pipeAdapter;
    event.events = 0;

    event.events |= EPOLLIN | EPOLLET;   // pipe is input only, Edge triggered.

    //int epoll_ctl(int epfd, int op, int fd,struct epoll_event *_Nullable event);
    if(epoll_ctl(pollFd, EPOLL_CTL_ADD, pipe.fd(), &event) == -1){
         std::cerr << "Unable to register input pipe with EPOLL " << strerror(errno) << std::endl;
        return 2;
    }       

    SerialEpollAdapter serialAdapter(&merge, &serial);
    event.data.ptr = &serialAdapter;
    event.events |= EPOLLOUT;   // serial needs to write too.

    //int epoll_ctl(int epfd, int op, int fd,struct epoll_event *_Nullable event);
    if(epoll_ctl(pollFd, EPOLL_CTL_ADD, serial.fd(), &event) == -1){
         std::cerr << "Unable to register serial port with EPOLL " << strerror(errno) << std::endl;
        return 3;
    }       
   
    struct epoll_event events[2];
    while(true){
        // use epoll wait to see what can be done and read/write data and pass to the
        // merge object.
        int count = epoll_wait(pollFd, events, 2, 500);
        if(count < 0) {
            std::cerr << "EPOLL Wait failed " << strerror(errno) << std::endl;
            return 4;
        }
        for(int i=0; i<count; ++i){
            EpollAdapter* handler = reinterpret_cast<EpollAdapter*>( events[i].data.ptr);
            handler->onEvent(events[i].events);
        }   
    }

    return 0;
}