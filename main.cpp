#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <cstdlib>

//network stuff
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sstream>

//trimming
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace std;

class INIParser {
private:

    string filename;
public:
    map<string, map<string, string> > properties;
    INIParser(string f) {
        filename = f;
        Parse();
    }

    string getProperty(string section, string propertyName) {
        return properties[section][propertyName];

    }

    int getSectionCount() {
        return properties.size();
    }

    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }

    void Parse() {
        ifstream langstream(filename.c_str());
        string currSection = "";
        string temp;
        while(getline(langstream, temp)) {
            temp = trim(temp);
            if(temp == "") continue;

            if(temp.size() > 2 && temp[0] == '[' && temp[temp.size() - 1] == ']') {
                currSection = temp.substr(1, temp.size() - 2);
                continue;
            }

            if(currSection == "") {
                cout << "ERROR: NO CURRENT LANGUAGE\n";
                break;
            }

            vector<string> tokens;
            int index = -1;
            if(temp.find('=') != string::npos) {
                index=temp.find('=');
                tokens.push_back(temp.substr(0,index));
                tokens.push_back("=");
                tokens.push_back(temp.substr(index+1));
            } else {
                cout << "ERROR: NO EQUAL IN PROPERTY\n";
                break;
            }

            properties[currSection][tokens[0]] = tokens[2];

        }
        langstream.close();
    }
};

class Service {
public:
    Service(string n, string t, string l, string s) {
        name = n;
        type = t;
        language = l;
        sourceFile = s;
        targetFile = "";
        successFlag = false;
    }


    void run(string parameters="") {
        compile();
        cout << targetFile + " " + "parameters: " << parameters << endl;
        system((targetFile + " " + parameters + " > tempServiceOutput.txt " + "2> tempError.log").c_str());
        processErrorLog();
       if(successFlag) {
           cout << "true";
       } else {
           cout << "false";
       }
       cout << endl;
    }

    bool isSuccessful() {
        return successFlag;
    }
    private:

    void compile() {
        if(language == "c") {
            system(("gcc " + sourceFile + " -o " + name).c_str());
            targetFile = "./" + name;
            return;
        }

        if(language == "c++" ) {
            system(("g++ " + sourceFile + " -o " + name).c_str());
            targetFile = "./" + name;
            return;
        }
        if(language == "python") {
            targetFile = "python ./" + sourceFile;
            return;
        }
        if(language == "perl") {
            targetFile = "perl ./" + sourceFile;
            return;
        }

    }

    void processErrorLog() {
    ifstream errorfile("tempError.log");
        if(errorfile.peek() == std::ifstream::traits_type::eof()) {
            cout << "No Error" << endl;
            successFlag = true;
        }
    ofstream ofs;
    ofs.open("tempError.log", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    }

private:
    string name;
    string type;
    string language;
    string sourceFile;
    string targetFile;
    bool successFlag;
};

class Services {
private:
    map<string, map<string, string> > ServicesVector;
public:
    Services() {
        //        INIParser LanguageData("languages.ini");

        INIParser ServicesData("services.ini");
        ServicesVector = ServicesData.properties;
    }

    string executeService(string serviceName) {
        string ret ="";
        if(ServicesVector.find(serviceName) == ServicesVector.end()) {
            return "Service Unavailable";
        }

        if(ServicesVector[serviceName]["parameterCount"] == "0") {

            Service currentService(serviceName, ServicesVector[serviceName]["type"],ServicesVector[serviceName]["language"], ServicesVector[serviceName]["filename"]);
            currentService.run();
            if(currentService.isSuccessful() == false) {
                return "Service Failed";
            }
            ifstream inputStream("tempServiceOutput.txt");
            string temp;
            while(getline(inputStream, temp)) {
                ret += temp;
            }
            return ret;
        } else {
            return getParameterList(serviceName);
        }

    }
    string executeService(string serviceName, vector<string> parameterList) {
        string ret ="";
        if(ServicesVector.find(serviceName) == ServicesVector.end()) {
            return "Service Unavailable";
        }

        if(parameterList.size() != stoi(ServicesVector[serviceName]["parameterCount"])) {
            return "Parameters count doesn't match";
        }

        string parameters="";
       // cout << parameterList.size();
        for(int i=0; i < parameterList.size(); i++) {
            if(ServicesVector[serviceName]["parameter_" + to_string(static_cast<long long>(i+1)) + string("_type")] == "string") {
                parameters += "\"" + parameterList[i] + "\" ";
            }
        }

        Service currentService(serviceName, ServicesVector[serviceName]["type"],ServicesVector[serviceName]["language"], ServicesVector[serviceName]["filename"]);
        currentService.run(parameters);
        if(currentService.isSuccessful() == false) {
            return "Service Failed";
        }
        ifstream inputStream("tempServiceOutput.txt");
        string temp;
        while(getline(inputStream, temp)) {
            ret += temp;
        }
        return ret;
    }

    string getParameterList(string serviceName) {
        int parameterCount = stoi(ServicesVector[serviceName]["parameterCount"]);
        string ret = "{\"parametersCount\":" + ServicesVector[serviceName]["parameterCount"] + ",";
        ret += "\"parametersRequired\":[";
        for(int i=1; i <= parameterCount; i++) {
            ret += "{";
//            ret += "\"ID\":" + to_string(i) + ",";
            ret += "\"Name\":\"" + ServicesVector[serviceName]["parameter_" + to_string(static_cast<long long>(i)) + string("_name")]+ "\",";
            ret += "\"Type\":\"" + ServicesVector[serviceName]["parameter_" + to_string(static_cast<long long>(i)) + string("_type")]+ "\"";
            ret += "},";
        }

        if(ret.size() > 1 && ret[ret.size()-1] == ',') {
            ret = ret.substr(0,ret.size()-1);
        }
        ret += "]}";

        return ret;
    }

    string getServicesNames() {
        string ret= "";
        typedef std::map<std::string, std::map<std::string, std::string> >::iterator it_type;
        for(it_type iterator = ServicesVector.begin(); iterator != ServicesVector.end(); iterator++) {
            ret += iterator->first + ',';
        }

        if(ret[ret.size()-1] == ',') {
            ret = ret.substr(0, ret.size()-1);
        }
        return ret;
    }
};


string cleanArray(char arr[], int n) {
    string ret="";
    for(int i=0; i < n; i++) {
        if(arr[i] != '#') {
            ret += arr[i];
        } else {
            break;
        }
    }
    return ret;
}

class ConnectionManager {
private:
    int listenfd,connfd;

    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    int numrv;
public:
    ConnectionManager(string ip) {
        listenfd = connfd = 0;
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        printf("socket retrieve success\n");
        int t = 1;
        setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));

        memset(&serv_addr, '0', sizeof(serv_addr));
        memset(sendBuff, '\0', sizeof(sendBuff));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        serv_addr.sin_port = htons(8888);
        bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

        if(listen(listenfd, 10) == -1){
            printf("Failed to listen\n");
        }
    }

    void Listen(Services availableServices) {
        while(1)
        {

            memset(sendBuff, '\0', sizeof(sendBuff));

            connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); // accept awaiting request

            char recBuff[1025];

            memset(recBuff, '#', 1025);
            read(connfd, recBuff, strlen(recBuff));



            string recieved = cleanArray(recBuff, sizeof(recBuff));
                            cout << "recieved: " << recieved << endl;

            string JSONResult;
            if(recieved == "GetServiceNames") {
                JSONResult = availableServices.getServicesNames();
            } else {
                if(recieved.find(',') != string::npos) {
                    vector<string> parameterList;
                    int index = recieved.find(',');
                    string serviceName = recieved.substr(0, index);
                    string parameters = recieved.substr(index+1);

                    stringstream ss(parameters);

                    string token;

                    while(std::getline(ss, token, ',')) {
                        parameterList.push_back(token);
                    }
                    cout <<"LOL1" << endl;
                    JSONResult = availableServices.executeService(serviceName, parameterList);
                } else {
                    cout << "LOL2" << endl;
                    JSONResult = availableServices.executeService(recieved);
                }

            }

            cout << "sent: " << JSONResult << endl;

            for(int i=0; i < JSONResult.size(); i++) {
                sendBuff[i] = JSONResult[i];
            }

            //        strcpy(sendBuff, "Message from server");


            write(connfd, sendBuff, strlen(sendBuff));

            close(connfd);
            sleep(1);

        }

    }



};

int main() {
    Services currentServices = Services();
    ConnectionManager connect = ConnectionManager("0.0.0.0");
    connect.Listen(currentServices);
}
