#include <vector>
#include <stdio.h>
#include <queue>
#include <string.h>
#include <map>
#include <mutex>
#include <time.h>
#include "httpclient.h"

const int  time_frame = 5;
const int max_clients = 5;
int server_port = 8080;

class ClientInfo
{
public:
    bool client_request()
    {
        std::lock_guard<std::mutex> lock(ctxMutex);
        
        time_t now_time = time(NULL);

        maintain_deadlines( now_time );

        if (deadlines.size() >= max_clients)
        {
            return false;
        }

        time_t deadline = now_time + time_frame;
        deadlines.push( deadline );

        return true;
    }

private:

    void maintain_deadlines(time_t now_time)
    {
        while(!deadlines.empty())
        {
            if (deadlines.top() <= now_time)
            {
                deadlines.pop();
            }
            else
            {
                break;
            }
        }
    }

    typedef std::priority_queue<time_t, std::vector<time_t>, std::greater<time_t> > Queue_t;

    int client_id;
    Queue_t deadlines;
    std::mutex ctxMutex;

};


class Server
{
public:

    bool run_server()
    {
        if (!init_listener( server_port ))
        {
            fprintf(stderr, "init_listener failed\n");
            return false;
        }


        fprintf(stderr,"server listening... socket %d\n", sockfd_);
        while( true )
        {
            struct sockaddr_in their_addr;

            socklen_t sin_size = sizeof(their_addr);
            int new_fd;

            if ((new_fd = accept(sockfd_, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
                fprintf(stderr,"accpept failed errno %d\n", errno);
                exit(1);
            }

            fprintf(stderr,"accept sock=%d\n", new_fd);
            start_receiver( new_fd );
        }   
    }
private:

    typedef std::pair<int, Server *> Message;
    static void *receiver_thread( void *arg )
    {
        Message *msg = (Message *) arg;

        fprintf(stderr,"start receiver thread fd %d\n", msg->first);

        int fd = msg->first;
        Server *inst = msg->second;

        delete msg;

        http_client client;
        
        client.set_fd(fd);

        int client_id = -1;

        if (!client.recv_request( &client_id ))
        {
            fprintf(stderr,"failed to get request\n");
        }

        fprintf(stderr, "got client request client: %d\n", client_id);

        bool allowed = false;
        
        if (client_id > 0)
        {
            allowed = inst->on_client_request( client_id );
        }

        fprintf(stderr,"client_id %d allowed: %d\n", client_id, allowed);


        if (!client.send_response( allowed ? HTTP_OK : HTTP_NO_SERVICE ))
        {
            fprintf(stderr,"failed to send response: client %d\n", client_id);
        }
        
    }

    void start_receiver(int new_fd )
    {
        pthread_t tid;

        Message *msg = new Message( new_fd, this );
        
        pthread_create( &tid, NULL, receiver_thread, (void *) msg);
        //receiver_thread(msg);
    }

    bool init_listener(int server_port)
    {
        if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            fprintf(stderr,"socket failed\n");
            exit(1);
        }

        struct sockaddr_in my_addr;
        
        memset(&(my_addr.sin_zero), 0, sizeof(my_addr) );        /* zero the rest of the struct */

        my_addr.sin_family = AF_INET;         /* host byte order */
        my_addr.sin_port = htons(server_port);     /* short, network byte order */
        my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
        
        if (bind(sockfd_, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))  < 0) {
            fprintf(stderr,"bind failed\n");
            exit(1);
            return false;
        }
        
        int option=1;
        if(setsockopt(sockfd_,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
        {
            fprintf(stderr,"failed to set reuseport\n");
        }

        listen(sockfd_,5);

        return true;
    }


    bool on_client_request(int client_id)
    {
        ClientInfo *info;

        {
            std::lock_guard<std::mutex> lock(mapMutex);
            
            auto it =  clients.find( client_id );
            if (it == clients.end())
            {
                info = new ClientInfo();
                clients.insert( MapClientIdToClient::value_type(client_id, info) );
            }
            else
            {
                info = it->second;
            }
    
        }
        return info->client_request();

    }

    typedef std::map<int, ClientInfo*> MapClientIdToClient;
    std::mutex mapMutex;

    MapClientIdToClient clients;
    int sockfd_;
};

int main()
{
    Server server;

    server.run_server();
    
    fprintf(stderr,"server exit\n");
    return 0;
}
