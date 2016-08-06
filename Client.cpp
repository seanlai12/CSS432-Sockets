/*
 * Client.cpp
 *
 *  Created on: Oct 7, 2014
 *      Author: saibadey
 */


#include <Socket.h>
#include <signal.h>
#include <fcntl.h>


int main( int argc, char* argv)
{


    
    
    
	int port = argv[1];
	int repetition = argv[2];
	int nbufs = argv[3];
	int bufsize = argv[4];
	int serverIp = argv[5];
	int type = atoi(argv[6]);

	//open a new socket and establish connection to server

	int sd = socket(AF_INET, SOCK_STREAM, 0);
    
    
	fcntl(sd, F_SETOWN, getpid( )); 	// ask OS to deliver this fd interrupt to me
	fcntl(sd, F_SETFL, FASYNC);	// set this fd asynchronous

	char databuf[nbufs][bufsize];

	//start timer

	if(type == 1)
	{
		//multiple write
		write( sd, databuf, nbufs * bufsize );    // sd: socket descriptor
	}
	else if (type == 2)
	{
		//write v
//		struct iovec vector[nbufs];
//		for ( int j = 0; j < nbufs; j++ ) {
//		    vector[j].iov_base = databuf[j];
//		    vector[j].iov_len = bufsize;
//		}
//		writev( sd, vector, nbufs );           // sd: socket descriptor
	}

	//Lap the timer by calling gettimeofday, where lap - start = data-sending time.
	//int dataSendingTime = gettimeofday() - start;

	//receive integer acknowledgment of how many reads were called

	//stop timer

	//cout stats:
	//Test 1: data-sending time = xxx usec, round-trip time = yyy usec, #reads = zzz


	close (sd);
	return 0;
}

