#include<stdio.h>
#include<netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

int main()
{
    struct hostent *ghbn=gethostbyname("www.iitd.ac.in");//change the domain name
    if (ghbn) {
        cout << inet_ntoa(*(struct in_addr *)ghbn->h_addr) << endl;
    }
}