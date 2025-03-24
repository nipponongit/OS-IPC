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
// / Class for IPC Framework, handling Pipes, Message Queues, Shared Memory, and Sockets
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
#include "../include/ipc_framework.h"

using namespace std;

#define Pipe_Name "/tmp/my_pipe"
#define Shm_Key 1234
#define Port 8080

// Pipes
void IPCFramework::createPipe() {
    mkfifo(Pipe_Name, 0666);
}

void IPCFramework::writePipe(const string& message) {
       int df = open(Pipe_Name, O_WRONLY);
   write(df, message.c_str(), message.size());
      close(df);
}

void IPCFramework::readPipe() {
 char buffer[256];
 int df = open(Pipe_Name, O_RDONLY);
 read(df, buffer, sizeof(buffer));
 close(df);
 cout << "Received via Pipe: " << buffer << endl;
}

// Message Queue
int IPCFramework::createMessageQueue() {
return msgget(IPC_PRIVATE, IPC_CREAT | 0666);
}

void IPCFramework::sendMessageQueue(int msgid, const string& message) {
    Message msg;
    msg.msg_type = 1;
    strncpy(msg.msg_text, message.c_str(), sizeof(msg.msg_text));
    msgsnd(msgid, &msg, sizeof(msg), 0);
}

void IPCFramework::receiveMessageQueue(int msgid) {
    Message msg;
    msgrcv(msgid, &msg, sizeof(msg), 1, 0);
    cout << "Received via Message Queue: " << msg.msg_text << endl;
}

// Shared Memory
int IPCFramework::createSharedMemory() {
    return shmget(Shm_Key, 256, IPC_CREAT | 0666);
}

void IPCFramework::writeSharedMemory(int shmid, const string& message) {
    char* abc = (char*)shmat(shmid, nullptr, 0);
    strncpy(abc, message.c_str(), 256);
    shmdt(abc);
}

void IPCFramework::readSharedMemory(int shmid) {
    char* abc = (char*)shmat(shmid, nullptr, 0);
    cout << "Received via Shared Memory: " << abc << endl;
    shmdt(abc);
}

// Sockets
void IPCFramework::startServer() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(Port);
    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    int client_fd = accept(server_fd, nullptr, nullptr);
    char buffer[256] = {0};
    read(client_fd, buffer, 256);
    cout << "Received via Socket: " << buffer << endl;
    close(server_fd);
}

void IPCFramework::startClient() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    string message = "Hello Server!!";
    send(sock, message.c_str(), message.size(), 0);
    close(sock);
}
#include "../include/ipc_framework.h"
#include <thread>

using namespace std;

int main() {
    cout << "Starting IPC Framework Test...\n";

    // Test Pipes
    cout << "\n[Testing Pipes]\n";
    IPCFramework::createPipe();
    thread writer([] { IPCFramework::writePipe("Hello from Pipe!"); });
    thread reader([] { IPCFramework::readPipe(); });
    writer.join();
    reader.join();

    
    cout << "\n[Testing Message Queue]\n"; //test message queue
    int msgid = IPCFramework::createMessageQueue();
    IPCFramework::sendMessageQueue(msgid, "Hello from Message Queue!");
    IPCFramework::receiveMessageQueue(msgid);

    // Test Shared memory
    cout << "\n[Testing Shared Memory]\n";
    int shmid = IPCFramework::createSharedMemory();
    IPCFramework::writeSharedMemory(shmid, "Hello from Shared Memory!");
    IPCFramework::readSharedMemory(shmid);

    // Test Socket
    cout << "[Testing Sockets]\n";
    thread serverThread([] { IPCFramework::startServer();});
    this_thread::sleep_for(chrono::seconds(1));
    thread clientThread([] { IPCFramework::startClient();});
    serverThread.join();
    clientThread.join();

    return 0;
}

