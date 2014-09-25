#include "server.h"

Server::Server()
{
  bzero((char *) &m_address, sizeof(m_address));
  m_address.sin_family = AF_INET;
  m_address.sin_addr.s_addr = INADDR_ANY;
  m_address.sin_port = htons(PORT);
}

Server::Server(std::string port)
{
  bzero((char *) &m_address, sizeof(m_address));
  m_address.sin_family = AF_INET;
  m_address.sin_addr.s_addr = INADDR_ANY;
  m_address.sin_port =
    htons((m_port = atoi(port.c_str())));
}

Server::~Server(){}

void Server::error(const char* message)
{
  perror(message);
}

int Server::createSocket()
{
  int socketId;
  if((socketId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    error("Error creating socket.");
  return socketId;
}

void Server::Close()
{
  close(m_connectionSocket);
  close(m_acceptSocket);
}

void Server::printHelp()
{
  //TODO
}

int Server::Send(std::string msg)
{
  zeroBuffer();
  if( sizeof(msg) > 255 )
  {
    fprintf(stderr, "ERROR : Message cannot send. Message is too long.");
    return -1;
  }
  strcpy(buffer, msg.c_str());
  printf("SEND");
  m_send = send(m_acceptSocket, buffer, sizeof(buffer), 0);
  if( m_send < 0 )
  {
    fprintf(stderr, "Message send failure.");
    return -1;
  }
  return 0;
}

int Server::Receive()
{
  zeroBuffer();
  m_receive = recv(m_acceptSocket, buffer, 255, 0);
  if( m_receive < 0 )
  {
    fprintf(stderr, "ERROR reading message.");
    return -1;
  }
  printf("RECEIVE : %s\n",buffer);
  return 0;
}

void Server::zeroBuffer()
{ bzero(buffer, 256); }

void Server::Listen()
{
  if(bind((m_connectionSocket = createSocket()),
      (struct sockaddr *) &m_address,
       sizeof(m_address)) < 0)
    error("ERROR on binding");
  else
  {
    listen(m_connectionSocket, 5);
    printf("Server is now listening on port %d.\n Type [quit] to shutdown server.\n", m_port);
    m_clientLength = sizeof(m_clientAddress);
    m_acceptSocket = accept(m_connectionSocket,
                            (struct sockaddr *) &m_clientAddress, &m_clientLength);
    if(m_acceptSocket < 0)
    {
      fprintf(stderr, "ERROR on accept.\n");
    }
    else
    {
      std::string line;
      while(line != "quit")
      {
        if(Receive() == 0)
        {
          std::getline(std::cin, line);
          if( !line.empty() || line != "quit" )
            Send(line);
        }
      }
    }
  }
  Close();
}
