#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include <bits/stdc++.h>
#include<netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>

#define TTL_LIMIT 255
using namespace std;

struct Router{
    string ip;
    int ttl;
    int rtt_min, rtt_avg, rtt_max, rtt_mdev;

    Router() {
        ip = "";
        ttl = rtt_min = rtt_avg = rtt_max = rtt_mdev = 0;
    } 
};

struct TraceRoute{
    vector<Router>path;

    void printPath() {
        return;
    }

    void add(Router r) {
        path.push_back(r);
    }

    Router get(int i) {
        if(i < 0 || i >= path.size()) throw logic_error("Out of bounds\n");
        else return path[i];
    }

    void update(Router r, int i) {
        if(i < 0 || i >= path.size()) throw logic_error("Out of bounds\n");
        else path[i] = r;
    }

    void plot(){
        return;
    }

    int length() {
        return path.size();
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


vector<string> parse(string currentRouter) {
    ofstream fout;        
    fout.open("sample.txt");
    fout << currentRouter << endl;
    fout.close();

    Py_Initialize();
    PyObject* myModuleString = PyUnicode_FromString("parser");
    PyObject* myModule = PyImport_Import(myModuleString);
    PyObject* myFunction = PyObject_GetAttrString(myModule, (char *)"parse");
    cout << "1\n";
    PyObject* pTup = PyObject_CallObject(myFunction, NULL);
    cout << "2\n";

    //convert result to vector
    vector<string>data;
    if(PyTuple_Check(pTup)) {
        for(Py_ssize_t i = 0; i < PyTuple_Size(pTup); i++) {
			PyObject *value = PyTuple_GetItem(pTup, i);
			data.push_back(PyUnicode_AsUTF8(value));
		}
    } else {
		throw logic_error("Passed PyObject pointer was not a list or tuple!");
	}
    Py_Finalize();

    return data;
}

void traceroute(string destDomain, string destRouter) { // (ip, ping result)
    int ttl = 1;    
    TraceRoute tr;
    string currentRouter = "";

    while(!targetReached(destRouter, currentRouter)) {
        currentRouter = ping(destDomain, ttl);
        cout << "Hello\n";

        vector<string>data = parse(currentRouter);

        cout << "data values\n";

        for(auto x : data) {
            cout << x << ' '; 
        }
        cout << endl;

        if(!currentRouter.empty()) {
            Router r;
            if(data.size() == 1) {
                r.ip = data[0];
            } else {
                r.ip = data[0];
                r.ttl = ttl;
                r.rtt_min = stoi(data[1]);
                r.rtt_avg = stoi(data[2]);
                r.rtt_max = stoi(data[3]);
                r.rtt_mdev = stoi(data[4]);
            }
            tr.add(r);
        } else {
            // some exception maybe
            throw logic_error("Some Problem\n");
            return;
        }
        ttl++;
    }
    
    // ttl = 1;
    // for(int i = 0; i < tr.length(); i++) {
    //     currentRouter = ping(tr.get(i).ip, ttl);
    //     vector<string>data = parse(currentRouter);
    //     if(!currentRouter.empty()) {
    //         Router r;
    //         r.ip = tr.get(i).ip;
    //         r.rtt_min = stoi(data[1]);
    //         r.rtt_avg = stoi(data[2]);
    //         r.rtt_max = stoi(data[3]);
    //         r.rtt_mdev = stoi(data[4]);
    //         r.ttl = ttl;
    //         tr.update(r, i);
    //     } else {
    //         throw logic_error("Some Problem\n");
    //     }
    //     ttl++;
    // }
    
    // tr.plot();
}

int main() {
    string inputDomain;
    cin >> inputDomain;
    
    string ping_result = ping(inputDomain, TTL_LIMIT);

    if(!DNSExists(ping_result)) {
        cout << "Domain Name Exists\n";
        traceroute(DN2IP(inputDomain), ping_result);
    } else {
        cout << "Given domain name does not exist\n";
    }

    return 0;
}