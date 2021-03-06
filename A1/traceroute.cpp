#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include <bits/stdc++.h>
#include<netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <chrono>

#define TTL_LIMIT 255
using namespace std;

struct Router{
    string ip;
    int ttl;
    double rtt;

    Router() {
        ip = "";
        ttl = rtt = 0;
    } 
};

struct TraceRoute{
    vector<Router>path;

    void printPath() {
        for(int i = 0; i < path.size(); i++) {
            cout << "Hop: " << i + 1 << endl;
            Router r = path[i];

            cout << "IP: " << r.ip << endl;
            cout << "rtt: " << r.rtt << endl;

            cout << "-----------------------" << endl;
        }
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
        ofstream file;
        file.open("dataPoints.csv");
        file << "hops,rtt,ip\n";
        for(int i = 0; i < path.size(); i++) {
            file << path[i].ttl << "," << path[i].rtt << "," << path[i].ip << endl;
        }
        file.close();

        PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *presult;
        pName = PyUnicode_FromString((char*)"helper");
        pModule = PyImport_Import(pName);
        pDict = PyModule_GetDict(pModule);
        pFunc = PyDict_GetItemString(pDict, (char*)"plot");

        if (PyCallable_Check(pFunc)) {
            pValue=Py_BuildValue("(z)",(char*)"something");
            presult=PyObject_CallObject(pFunc,NULL);
            PyErr_Print();
        } else {
            PyErr_Print();
        }
        int status = PyLong_AsLong(presult);

        if(status != 1) {
            throw logic_error("Error in Plotting\n");    
        } 
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

vector<string> listTupleToVector_String(PyObject* incoming) {
	vector<string> data;
	if (PyTuple_Check(incoming)) {
		for(Py_ssize_t i = 0; i < PyTuple_Size(incoming); i++) {
			PyObject *value = PyTuple_GetItem(incoming, i);
			data.push_back( PyUnicode_AsUTF8(value) );
		}
	} else {
		if (PyList_Check(incoming)) {
			for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++) {
				PyObject *value = PyList_GetItem(incoming, i);
				data.push_back( PyUnicode_AsUTF8(value) );
			}
		} else {
			throw logic_error("Passed PyObject pointer was not a list or tuple!");
		}
	}
	return data;
}

void initEnv() {
    setenv("PYTHONPATH",".",1);
    Py_Initialize();
}

void endEnv() {
    Py_Finalize();
}

vector<string> parse(string currentRouter) {
    ofstream fout;        
    fout.open("sample.txt");
    fout << currentRouter << endl;
    fout.close();

    PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *presult;
    pName = PyUnicode_FromString((char*)"helper");
    pModule = PyImport_Import(pName);
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, (char*)"parse");

    if (PyCallable_Check(pFunc))
    {
        pValue=Py_BuildValue("(z)",(char*)"something");
        presult=PyObject_CallObject(pFunc,NULL);
        PyErr_Print();
    } else 
    {
        PyErr_Print();
    }
    vector<string> data = listTupleToVector_String(presult);
    Py_DECREF(pValue);
    Py_DECREF(pModule);
    Py_DECREF(pName);

    return data;
}

void traceroute(string destIP, string destRouter) { // (ip, ping result)
    int ttl = 1;    
    TraceRoute tr;
    string currentRouter = "";
    string currentIP = "";

    initEnv();

    while(true) {
        auto start = chrono::high_resolution_clock::now();
        currentRouter = ping(destIP, ttl);
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

        vector<string>data = parse(currentRouter);
        currentIP = data[0];
        if(!currentRouter.empty()) {
            Router r;
            r.ip = data[0];
            r.ttl = ttl;
            r.rtt = duration.count();
            tr.add(r);
        } else {
            throw logic_error("Some Problem\n");
            return;
        }
        ttl++;

        if(currentIP == destIP || ttl > 256) {
            break;
        }
    }
    tr.plot();
    endEnv();
}

int main() {
    string inputDomain;
    cin >> inputDomain;
    
    string ping_result = ping(DN2IP(inputDomain), TTL_LIMIT);

    if(!DNSExists(ping_result)) {
        cout << "Domain Name Exists\n";
        traceroute(DN2IP(inputDomain), ping_result);
    } else {
        cout << "Given domain name does not exist\n";
    }

    return 0;
}