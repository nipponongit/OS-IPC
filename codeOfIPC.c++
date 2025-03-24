#ifndef IPC_FRAMEWORK_H
#define IPC_FRAMEWORK_H

#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

// Structure for Message Queue communication
struct Message {
    long msg_type;
    char msg_text[256];
};

class IPCFramework {
public:
    // Pipes
    static void createPipe();
    static void writePipe(const string& message);
    static void readPipe();

    // Message Queue
    static int createMessageQueue();
    static void sendMessageQueue(int msgid, const string& message);
    static void receiveMessageQueue(int msgid);

    // Shared Memory
    static int createSharedMemory();
    static void writeSharedMemory(int shmid, const string& message);
    static void readSharedMemory(int shmid);

    // Sockets
    static void startServer();
    static void startClient();
};

#endif 