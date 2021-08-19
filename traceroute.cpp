#include <bits/stdc++.h>
using namespace std;

string ping(string domain) {
    string command = "ping " + domain + " -c 1";
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

int main() {
    string inputDomain;
    cin >> inputDomain;
    
    string result = ping(inputDomain);

    if(!DNSExists(result)) {
        cout << "Domain Name Exists\n";
    } else {
        cout << "Given domain name does not exist\n";
    }

}