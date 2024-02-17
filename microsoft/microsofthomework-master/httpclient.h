#ifndef _HTTP_CLIENT_
#define _HTTP_CLIENT_

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "httpclient.h"


const int REC_BUF = 1024;

const int HTTP_OK = 200;
const int HTTP_NO_SERVICE = 503;


// blocking client at that.
class http_client
{
public:
    http_client(bool trace_on = false) : sock_(-1), trace_on_(trace_on)
    {
    }

    bool connect(int port)
    {
        
        if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            return false;
        }

        struct sockaddr_in their_addr;

        memset(&(their_addr.sin_zero), 0, sizeof(their_addr));     /* zero the rest of the struct */

        their_addr.sin_family = AF_INET;      /* host byte order */
        their_addr.sin_port = htons(port);    /* short, network byte order */
        their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        if (::connect(sock_, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
            return false;
        }
        return true;


    }

    void set_fd(int fd) { sock_ = fd; }

    ~http_client() { close(); }

    void close()
    {
        if (sock_ != -1)
        {
            ::close(sock_);
            sock_ = -1;
        }
    }


    bool send_request(int client_id)
    {
        char req[ REC_BUF ];
        int ret;
        int len;

        snprintf(req, sizeof(req)-1,  "GET /?client_id=%d HTTP/1.1\r\nConnection: close\r\n\r\n", client_id);
        len = strlen(req);

        if (trace_on_)
            fprintf(stderr, "sending: %d\n%s\n", len, req);

        ret = send(sock_, req, len, 0 );
    
        return ret == len;
    }

    bool recv_request(int *client_id)
    {
        char req[ REC_BUF ];
        int rt;
        
        if ((rt = recvfrom(sock_, req, sizeof(req)-1, 0, NULL, NULL)) < 0)
            return false;

        req[rt] = '\0';

        if (trace_on_)
            fprintf(stderr,"received %d:\n%s\n---\n", rt, req);        
        
        if (strstr(req, "\r\n\r\n" ) == 0)
        {
            fprintf(stderr,"not a http request\n");
            return false;
        }

        int clid = -1;

        sscanf(req, "GET /?client_id=%d", &clid);

        *client_id = clid;

        return true;
    }

    bool send_response(int status)
    {
        char req[ REC_BUF ];
        int ret;
        int len;

        snprintf(req, sizeof(req)-1, "HTTP/1.1 %d\r\nConnection: closed\r\n\r\n", status);
        len = strlen(req);

        if (trace_on_)
            fprintf(stderr, "sending: %d\n%s\n", len, req);

        ret = send(sock_, req, len, 0 );

        return ret == len;
    }

    bool recv_response(int *status)
    {
        char buf[ REC_BUF ];
        int rt;
        

        //if (rt = recv(sock_, buf, sizeof(buf)-1, 0) < 0)
        if ((rt = recvfrom(sock_, buf, sizeof(buf)-1, 0, NULL, NULL)) < 0)
        {
            fprintf(stderr, "recv error\n");
            return false;
        }

        buf[rt] = '\0';

        if (trace_on_)
            fprintf(stderr,"received %d:\n%s\n---\n", rt, buf);

        if (strstr(buf, "\r\n\r\n" ) == 0)
        {
            fprintf(stderr,"no eof\n");
            return false;
        }

        int stat = -1;

        sscanf(buf, "HTTP/1.1 %d", &stat);

        *status = stat;
        
        return true;
    }

private:
    int sock_;
    bool trace_on_;
};


#endif
