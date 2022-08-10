#include <enet/enet.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

ENetAddress address;
ENetHost* server = nullptr;
ENetHost* client = nullptr;

bool CreateServer()
{
    address.host = ENET_HOST_ANY;
    address.port = 1234;
    server = enet_host_create(&address, 32, 2, 0, 0);
    return server != nullptr;
}
bool CreateClient()
{
    client = enet_host_create(NULL, 1, 2, 0, 0);
    return client != nullptr;
}

string chatName = "Anonymous";
bool messageReady = false;
char MessageName[99];

void MessageIntake()
{
    cin.ignore(999, '\n');
    while (1)
    {
        cin.getline (MessageName, 999, '\n');
        messageReady = true;
    }  
    this_thread::sleep_for(chrono::milliseconds(500));
}



int main(int argc, char** argv)
{
    // Initial Setup

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        cout << "An error occurred while initializing ENet." << endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    
    cout << "Enter Chat Name: ";
    getline(cin, chatName);
    cout << endl;

    cout << "1) Create Server " << endl;
    cout << "2) Create Client " << endl << endl;
    int UserInput;
    cin >> UserInput;

    // I don't know why MessageName gets obliterated the second it enters the loop
    cout << "Type your chat messages below!" << endl;
    cout << "Type QUIT to quit." << endl << endl;
    
    thread FirstThread(MessageIntake);
    FirstThread.detach();

    if (UserInput == 1)
    {
        if (!CreateServer())
        {
            fprintf(stderr,
                "An error occurred while trying to create an ENet server host.\n");
            exit(EXIT_FAILURE);
        }
    }
    if (UserInput == 2)
    {
        if (!CreateClient())
        {
            fprintf(stderr,
                "An error occurred while trying to create an ENet client host.\n");
            exit(EXIT_FAILURE);
        }

        ENetAddress address;
        ENetEvent event;
        ENetPeer* peer;

        enet_address_set_host(&address, "127.0.0.1");
        address.port = 1234;
        peer = enet_host_connect(client, &address, 2, 0);

        if (peer == NULL)
        {
            fprintf(stderr, "No available peers for initiating an ENet connection.\n");
            exit(EXIT_FAILURE);
        }

        if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            cout << "Connection to 127.0.0.1:1234 succeeded." << endl;
        }

        else
        {
            enet_peer_reset(peer);
            cout << "Connection to 127.0.0.1:1234 failed." << endl;
        }
    }



    // Host Code
    if (UserInput == 1)
    {
        while (1)
        {

            if (MessageName[0] == 'Q' && MessageName[1] == 'U' && MessageName[2] == 'I' && MessageName[3] == 'T')
            {
                cout << endl << endl << "Exiting Program" << endl << endl;
                return 0;
            }

            if (messageReady)
            {
                string Message = chatName + ": " + MessageName;
                cout << Message << endl << endl;
                ENetPacket* packet = enet_packet_create(Message.c_str(), Message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
                enet_host_broadcast(server, 0, packet);
                enet_host_flush(server);
                messageReady = false;
            }

            ENetEvent event;
            while (enet_host_service(server, &event, 500) > 0)
            {
                switch (event.type)
                {
                case ENET_EVENT_TYPE_CONNECT:
                    cout << "A new client connected. " << endl;
                    event.peer->data = (void*)("Client information");
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    cout << (char*)event.packet->data << endl;
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    cout << (char*)event.peer->data << "disconnected." << endl;
                    event.peer->data = NULL;
                }
            }

        }
    }

    // Client Code
    if (UserInput == 2)
    {
        while (1)
        {
            if (MessageName[0] == 'Q' && MessageName[1] == 'U' && MessageName[2] == 'I' && MessageName[3] == 'T')
            {
                cout << endl << endl << "Exiting Program" << endl << endl;
                return 0;
            }

            if (messageReady)
            {
                string Message = chatName + ": " + MessageName;
                cout << Message << endl << endl;
                ENetPacket* packet = enet_packet_create(Message.c_str(), Message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
                enet_host_broadcast(client, 0, packet);
                enet_host_flush(client);
                messageReady = false;
            }

            ENetEvent event;
            while (enet_host_service(client, &event, 500) > 0)
            {
                switch (event.type)
                {
                case ENET_EVENT_TYPE_RECEIVE:
                    cout << (char*)event.packet->data << endl;
                    enet_packet_destroy(event.packet);
                }
            }

        }
    }


    // Post-Program Clean-up

    else
    {
        cout << "Invalid Input" << endl;
    }
    if (server != nullptr)
    {
        enet_host_destroy(server);
    }
    if (client != nullptr)
    {
        enet_host_destroy(client);
    }

    return EXIT_SUCCESS;
}