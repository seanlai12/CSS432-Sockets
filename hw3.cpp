/*
 * HW3 TCP Analysis
 * file: hw3.cpp
 * Author:  Sean Lai
 * Date:    November 11, 2014
 * Class:   CSS 432 Fukuda
 *
 * Source code for mimicing the functionality of the hw3 tcp in regard of
 * sequence, direction of messages, # of messages, and the data amount.
 * Shuts down both server and client in terms of writing and then closes the
 * socket at the end of the transmission.
 */

#include "Socket.h"

using namespace std;

void server( ); 
void client( char ipName[] );
void usage( char progName[] );

Socket *sock;

int main( int argc, char* argv[] ) {
  if ( argc > 1 ) {
    sock = new Socket( atoi( argv[1] ) );
    if ( argc == 2 )
      server( );
    else if ( argc == 3 )
      client( argv[2] );
  }
  else {
    usage( argv[0] );
    return -1;
  }
  return 0;
}

// Server side TCP
void server( ) {

  // Get a server sd
  int serverSd = sock->getServerSocket( );

  // Exchange data
  char message[1500];
  read( serverSd, message, 10 ); // 1. Reads length 10
  write( serverSd, message, 10 ); // 2. Push length 10
    
  shutdown(serverSd, SHUT_WR); // Shuts down the write. Sends a Fin.
  read( serverSd, message, 1450); // 3. Server still can read. Reads length 1450
    
  // Close socket but not send FIN.
  close( serverSd );
}

// Client side TCP
void client( char ipName[] ) {

  // Get a client sd
  int clientSd = sock->getClientSocket( ipName );

  // Exchange data
  char message[1500];
  write( clientSd, message, 10 ); // 1. Push a length 10 data to server
  read( clientSd, message, 10 ); // 2. reads length 10 from server
  write( clientSd, message, 1450 ); // 3. Push a length 1450 to server
    // Ack received is automatic.
    
  shutdown(clientSd, SHUT_WR); // Shuts down the write. Sends a Fin.
    
  // Close socket to send FIN.
  close( clientSd );
}

void usage( char progName[] ) {
  cerr << "usage:" << endl;
  cerr << "server invocation: " << progName << " ipPort" << endl;
  cerr << "client invocation: " << progName << " ipPort ipName" << endl;
}
