#include"header.h"
using namespace std;

std::string fileContents;
char* extractHost(char forHost[])
{
    char *hostName;
    hostName=strtok(forHost,"@");
    hostName=strtok(NULL,":");
    return hostName;

}

int readFile(string fname)
{
    std::ifstream file(fname.c_str());
    std::stringstream buffer;

    buffer << file.rdbuf();
    fileContents = buffer.str();
    return 0;

}

//Client side
int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        //cerr << "Usage: ip_address port" << endl;
        exit(0);
    } //grab the IP address and port number
    char *serverIp = argv[1];
    // int port = atoi(argv[2]);
    char *toMail = argv[2];
    char *subject = argv[3];
    char *txtFile =argv[4];
    //  cout<<subject<<txtFile<<endl;

    //extract the port from adress
    std::string user_port(toMail);
    char user_adr[100],forHost[100],user[100];
    string x = user_port.substr(user_port.find(":") + 1);
    int port = atoi(x.c_str());

    //extract the port from adress
    strcpy(user_adr,toMail);
    strcpy(user,toMail);
    strcpy(forHost,toMail);
    strtok(user_adr,":");
    strtok(user,"@");
    cout<<"User: "<<user<<endl;
    char *hostName = extractHost(forHost);
    /*hostName=strtok(forHost,"@");
    hostName=strtok(NULL,":");*/
    cout<<"Host: "<<hostName<<endl;
    char msg[1500];
    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;

    }
    else
        cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    string from="shuvo-HP-Pavilion-Notebook";
    bool isMailFrom=false,isMailTo=false,canSendData =false;
    string msgBody;
    while(1)
    {


        cout << ">";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, data.c_str());
        if(data == "QUIT")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        if(data == "MAIL FROM")
        {
            memset(&msg, 0, sizeof(msg));
            strcpy(msg, hostName);
            isMailFrom=true;

        }
        if(data == "RCPT TO" &&isMailFrom)
        {
            memset(&msg, 0, sizeof(msg));
            strcpy(msg, user_adr);
            // cout<<user_adr<<endl;
            isMailFrom=false;
            isMailTo=true;

        }
        if(data == "DATA" && isMailTo)
        {
            memset(&msg, 0, sizeof(msg));
            //  std::string str(subject);
            strcpy(msg, "DATA#");
            strcat(msg,subject);
            // cout<<user_adr<<endl;
            isMailTo=false;
        }

        //send from client
        send(clientSd, (char*)&msg, strlen(msg), 0);
        //clear the buffer
        memset(&msg, 0, sizeof(msg));
        //recv from client
        recv(clientSd, (char*)&msg, sizeof(msg), 0);

        //check status code
        if(!strcmp(msg, "221"))
        {
            cout <<msg<< " Service closing transmission channel" << endl;
            break;
        }
        else if(!strcmp(msg, "202"))
        {
            cout << msg<<" Server:Connection Request Accepted"<< endl;
        }

        else if(!strcmp(msg, "200"))
        {
            cout << msg<<" Server:Host name is correct."<< endl;
        }
        else if(!strcmp(msg, "250"))
        {
            cout << msg<<" Server:Requested mail action okay."<< endl;
        }
        else if(!strcmp(msg, "550"))
        {
            cout <<msg<< " Server:Requested action not taken: mailbox unavailable"<< endl;
        }
        else if(!strcmp(msg, "500"))
        {
            cout <<msg<< " Server:Syntax error or some ERROR occured."<< endl;
        }
        else if(!strcmp(msg, "354"))
        {
            cout <<msg<< " Server:Start Sending Mail."<< endl;
            canSendData=true;
        }
        else if(!strcmp(msg, "450"))
        {
            cout <<msg<< " Server:Unvalid mail adress."<< endl;
            canSendData=true;
        }
        if(canSendData)
        {

            //  freopen(txtFile,"r",stdin);
            readFile(txtFile);
            memset(&msg, 0, sizeof(msg));
            strcpy(msg, fileContents.c_str());
            //  cout<<fileContents;
            canSendData=false;

            send(clientSd, (char*)&msg, strlen(msg), 0);
            memset(&msg, 0, sizeof(msg));
            recv(clientSd, (char*)&msg, sizeof(msg), 0);
            if(!strcmp(msg, "222"))
            {
                cout <<msg<< " Server:Data received."<< endl;
            }
            else cout << "Server:Error."<< endl;



        }

    }
    gettimeofday(&end1, NULL);
    close(clientSd);
    cout << "Connection closed" << endl;
    return 0;
}
