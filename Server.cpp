/*
 * Server.cpp
 *
 *  Created on: Oct 7, 2014
 *      Author: saibadey
 */



#include <Socket.h>
#include <signal.h>
#include <fcntl.h>


using namespace std;

void performance(int stuff);

void performance(int stuff) {  // invoked upon an interrupt }

/*
 *
 * Allocate databuf[BUFSIZE], where BUFSIZE = 1500.
Start a timer by calling gettimeofday.
Repeat reading data from the client into databuf[BUFSIZE]. Note that the read system call may return without reading the entire data if the network is slow. You have to repeat calling read like:
       for ( int nRead = 0;
            ( nRead += read( sd, buf, BUFSIZE - nRead ) ) < BUFSIZE;
            ++count );
Check the manual page for read carefully.
Stop the timer by calling gettimeofday, where stop - start = data-receiving time.
Send the number of read( ) calls made, (i.e., count in the above) as an acknowledgment.
Print out the statistics as shown below:
     data-receiving time = xxx usec
Close this connection.
Terminate the server process by calling .
 *
 *
 */


	exit(0);
}



int main (int argc, char* argv[])
{

	int port = atoi( argv[1] );

	Socket socket( port );
	int fd = socket.getServerSocket();
//
//
//	signal(SIGIO, performance);	// set a future interrupt to call sigio_func( )
//	fcntl(fd, F_SETOWN, getpid( )); 	// ask OS to deliver this fd interrupt to me
//	fcntl(fd, F_SETFL, FASYNC);	// set this fd asynchronous
//

	char buffer[16];
	bzero( buffer, sizeof( buffer ) );
	int count = read( fd, buffer, 15 );

	write( fd, buffer, count);

	return 0;
}
