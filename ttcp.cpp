#include <iostream>

#include "Option.h"
#include "Socket.h"
#include "Timer.h"

using namespace std;

void help( ) {
  cerr << "Usage: ttcp -t [options] remotehost" << endl;
  cerr << "       ttcp -r [local options]" << endl;
  cerr << "       where:" << endl;
  cerr << "        -t      transmit data" << endl;
  cerr << "        -r      receive data" << endl;
  cerr << "       common options:" << endl;
  cerr << "        -l#     length of bufs read from or written to network (default 8192)" << endl;
  cerr << "        -b#     set the socket buffer size if supported (default is 16384)" << endl;
  cerr << "        -p#     specify another service port (default is 5001)" << endl;
  cerr << "        -?    print this help" << endl;
  cerr << "       options specific to -t:" << endl;
  cerr << "        -n#     number of source bufs written to network (default 2048)" << endl;
  cerr << "        -D      don't buffer TCP writes (sets TCP_NODELAY socket option)" << endl;
}

void result( const char *side, long time, int totalBytes, int iocalls ) {
  cerr << side << "bytes=" << totalBytes << " time=" << time 
       << " Mbps=" << float( totalBytes * 8 ) / time 
       << " I/Ocalls=" << iocalls << endl;
}

int atoi( char* str, int index, int &value ) {
  int count = 0;

  value = 0;
  for ( ; str[index] >= '0' && str[index] <= '9'; index++ ) {
    int c = str[index];
    value = value * 10 + ( c - '0' );
    count++;
  }

  return count;
}

bool checkOptions( int argc, char *argv[], Option &option ) {
  if ( argc <= 1 )
    return false;
  for ( int i = 1; i < argc; i++ ) {
    char c;
    if ( argv[i][0] != '-' ) {
      // remote host
      if ( option.host == NULL ) {
	option.host = argv[i];
	continue;
      } else
	return false;
    } else {
      // options
      for ( int j = 1; argv[i][j] != '\0'; j++ ) {
	switch ( argv[i][j] ) {
	case 't':
	  option.transmit = true;
	  break;
	case 'r':
	  option.receive = true;
	  break;
	case 'l':
	  j += atoi( argv[i], j + 1, option.buflen );
	  break;
	case 'b':
	  j += atoi( argv[i], j + 1, option.sockbufsize);
	  break;
	case 'p':
	  j += atoi( argv[i], j + 1, option.port );
	  break;
	case '?':
	  option.help = true;
	  break;
	case 'n':
	  j += atoi( argv[i], j + 1, option.nbuf );
	  break;
	case 'D':
	  option.tcp_nodelay = true;
	  break;
	default:
	  return false;
	} // end of case
      } // end of for
    } // end of else
  } // end of for
  if ( option.transmit == true && option.receive == true ||
       option.transmit == false && option.receive == false ||
       option.transmit == true && option.host == NULL || 
       option.help == true )
    return false;
  else
    return true;
}

int allWrite( int sd, char *buf, int size, int &iocalls ) {
  int nwritten = 0;
  int pos;

  for ( pos = 0; pos < size; pos += nwritten ) {
    nwritten = write( sd, buf + pos, size - pos );
    iocalls++;
  }
  return pos;
}

int allRead( int sd, char *buf, int size, int &iocalls ) {
  int nread = 1; // set to get into the next for loop
  int pos;

  for ( pos = 0; pos < size && nread > 0; pos += nread ) {
    nread = read( sd, buf + pos, size - pos );
    iocalls++;
  }
  return pos;
}

void transmit( Option &option ) {
  cout << "ttcp-t: buflen=" << option.buflen 
       << ", nbuf=" << option.nbuf
       << ", port=" << option.port
       << ", sockbufsize=" << option.sockbufsize
       << ", tcp -> " << option.host << endl;

  Socket socket( option.port );
  int sd = socket.getClientSocket( option.host, option.sockbufsize,
				   option.tcp_nodelay );
  if ( sd == NULL_FD ) {
    cout << "ttcp-t: failed in connecting to " << option.host << endl;
    return;
  }

  char buf[option.buflen];
  int totalBytes = 0;
  int iocalls = 0;

  // send the buffer length
  write( sd, &option.buflen, sizeof( int ) );

  Timer t;
  t.start( );
  for ( int i = 0; i < option.nbuf; i++ ) {
    totalBytes += allWrite( sd, buf, option.buflen, iocalls );
  }
  shutdown( sd, SHUT_WR );
  long clientTime = t.stop( );

  result( "ttcp-t", clientTime, totalBytes, iocalls );
  
  long serverTime;
  read( sd, ( char * )&serverTime, sizeof( long ) );
  read( sd, ( char * )&totalBytes, sizeof( int ) );
  read( sd, ( char * )&iocalls, sizeof( int ) );
  close( sd );

  result( "ttcp-r", serverTime, totalBytes, iocalls );
}

void receive( Option &option ) {
  Socket socket( option.port );
  while ( true ) {
    int sd = socket.getServerSocket( option.sockbufsize, option.tcp_nodelay );
    if ( sd == NULL_FD ) {
      cout << "ttcp-t: failed in accepting a new connection " << endl;
      return;
    }

    // receive the buffer length
    read( sd, &option.buflen, sizeof( int ) );

    char buf[option.buflen];
    int totalBytes = 0;
    int currentBytes = 0;
    int iocalls = 0;

    Timer t;
    t.start( );
    while ( true ) {
      totalBytes += (currentBytes = allRead(sd, buf, option.buflen, iocalls));
      if ( currentBytes < option.buflen )
	break;
    }
    long serverTime = t.stop( );

    write( sd, (char *)&serverTime, sizeof( long ) );
    write( sd, (char *)&totalBytes, sizeof( int ) );
    write( sd, (char *)&iocalls, sizeof( int ) );
    close( sd );
  }
}

int main( int argc, char *argv[] ) {
  Option option;
  if ( checkOptions( argc, argv, option ) == false ) {
    help( );
    return -1;
  }

  if ( option.transmit == true )
    transmit( option );
  else
    receive( option );
  return 0;
}
