# microsofthomework

This is a homework assignment for a Microsoft job interview; 
Anyway, i didn't get the job offer, so apparently I am doing more bugs than Microsoft can fathom.
Or something else, they didn't specify what was wrong.
Anyway, I think this homework style is better then  the google interview - that's just plain abuse (I can't talk, do non trivial code and do non trivial thinking all at the same time, sorry that's too much).

The task
========

1. Client

a. The user enters the number of HTTP clients to simulate.

b. For each HTTP client you will run a separate simultaneous thread/task which will do the following in a loop:

i. Send HTTP request to a server with simulated HTTP client identifier as a query parameter (e.g.http://localhost:8080/?clientId=3).

   The client identifier is a random integer between 1 to the number of clients. Different clients can have the same identifier.

ii. Wait some random time and then send another request.

iii. The client will run until key press after which it will gracefully drain all the threads/tasks (wait for all the of them to complete) and will exit.

2. Server

a. Starts listening for incoming HTTP requests.

b. For each incoming HTTP request you will do the following:

i. Handle the request in a separate thread/task.

ii. Check if this specific client reached the max number of requests per time frame threshold (no more than 5 requests per 5 secs).
                          Note: The time frame starts on each client’s first request and ends 5 seconds later. 

              After the time frame has ended, the client’s next first request will open a new time frame and so forth.

iii. If the client hasn’t reached the threshold, it will get HTTP response with status code 200 (OK) otherwise status code 503 (Service Unavailable).

c. The server will run until key press after which it will gracefully drain all the threads/tasks and will exit. 

3. General notes

a. Pay attention to thread safeness.

b. The solution should be as simple and clean as possible, avoid over design/engineering and stick to the requirements.''


The deliverable
===============

1. to compile:
      make
2. to run the client
      ./client -n 10 # number of clients
3. tun run the server
     ./server 

