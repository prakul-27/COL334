#include <bits/stdc++.h>
#include<netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include "Python.h"
using namespace std;

#define TTL_LIMIT 255

struct TraceRoute{
    vector<string>path; // path[i] contains router[i] info

    void printPath() {
        for(int i = 0; i < path.size(); i++) {
            cout << path[i];
        }
        cout << "------------------------" << endl;
    }
};

string ping(string domain, int ttl) {
    string command = "ping " + domain + " -4 " + " -c 1" + " -t " + to_string(ttl);
    const char* cmd = command.c_str();
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool DNSExists(string result) {
    return result.empty();
}

bool targetReached(string destDomain, string currentRouter) {
    return destDomain == currentRouter;
}

string DN2IP(string domainName) {
    struct hostent *ghbn=gethostbyname(domainName.c_str());
    if (ghbn) {
        return inet_ntoa(*(struct in_addr *)ghbn->h_addr);
    } else return "";    
}


void traceroute(string destDomain, string destRouter) {
    int ttl = 1;    
    TraceRoute tr;
    string currentRouter = "";

    while(!targetReached(destRouter, currentRouter)) {
        currentRouter = ping(destDomain, ttl);

        ofstream fout;        
        fout.open("sample.txt")
        fout << currentRouter << endl;
        fout.close();

        if(!currentRouter.empty()) {
            // tr.path.push_back(currentRouter); // add ip of currentRouter, parse result to get IP
        } else {
            // some exception maybe
            cout << "Some Problem\n";
            return;
        }
        ttl++;
    }

    tr.printPath();      
}

int main() {
    string inputDomain;
    cin >> inputDomain;
    
    string result = ping(inputDomain, TTL_LIMIT);

    if(!DNSExists(result)) {
        cout << "Domain Name Exists\n";
        // traceroute(inputDomain, result);
        string res = DN2IP(inputDomain);
        cout << res << endl;
    } else {
        cout << "Given domain name does not exist\n";
    }

    return 0;
}