#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include "httpclient.h"

int numclients = 0;
int server_port = 8080;
std::atomic<int> thread_count;
std::atomic<bool> stop_requested(false);

bool trace_on = false;


bool send_request(int client_id)
{
    http_client client;
    int status;

    if (!client.connect(server_port))
    {
        fprintf(stderr,"client %d failed to connect\n", client_id);
        exit(-1);
         
    }

    if (!client.send_request( client_id ))
    {
        fprintf(stderr,"client %d failed to send request\n", client_id);
        exit(-1);
    }

    if (!client.recv_response( &status ))
    {
        fprintf(stderr,"client %d failed to get response\n", client_id);
        exit(-1);
    }

    if (status != HTTP_OK && status != HTTP_NO_SERVICE)
    {
        fprintf(stderr, "client %d got unexpected status %d\n", client_id, status);
        exit(-1);
    }
    if (trace_on)
        fprintf(stderr,"client %d got status %d\n", client_id, status);

    return true;
}

void *client_thread(void *arg)
{
    int client_id;

    while( !stop_requested.load() )
    {
        client_id = rand() % numclients + 1;
        send_request(client_id);
        int num_sleep = rand() % 1000; // num of milliseconds to sleep
        usleep( num_sleep * 1000 );

    }

    thread_count -=1;

    return nullptr;
}


void start_clients(int numclients)
{
  for(int i =0; i < numclients; ++i)
  {     
        pthread_t tid;

        pthread_create( &tid, NULL, client_thread, NULL);
  }
}

void print_help()
{
    fprintf(stderr,"-n <numclients\noptional: -p <port>\noptional: -t\n");
    exit(1);
}

int main(int argc, char *argv[])
{
  int opt;

  while ((opt = getopt(argc, argv, "pn:th")) != -1) {
    switch (opt) {
       case 'n':
         numclients = atoi(optarg);
         break;
       case 'p':
         server_port = atoi(optarg);
         break;
       case 't':
         trace_on = true;
         break;
       case 'h':
         print_help();
         break;
    }
  }

  if (numclients <= 0)
  {
    fprintf(stderr,"no clients specified. set -n <num clients>\n");
    exit(-1);
  }

  fprintf(stderr,"num clients: %d port %d\n", numclients, server_port);

  start_clients(numclients);

  printf("print any key and enter to stop the test\n");
  getchar();
  stop_requested = true;
  
  printf("waiting for client threads to finish\n");
  while( thread_count.load() != 0)
  {
    usleep(1000);
  }
  printf("test completed.\n");

  return 0;
}

