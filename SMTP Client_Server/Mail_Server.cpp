#include"header.h"
using namespace std;
//Server side

//Thisfunction checkif user file exist or not
bool fileChecker(const char *fname)
{
    if(freopen(fname,"r",stdin)) return true;
    else return false;
}

//this function print the current time date
void printTime()
{


    time_t t;
    time(&t);

    printf("%s", ctime(&t));


}

//this function find the subject
char* extractSubject(char sub[])
{
    char *subj;
    subj=strtok(sub,"#");
    subj=strtok(NULL,"'\0'");
    return subj;

}
//this function find the hostname

char hostName[1024];
int findHostName()
{

    gethostname(hostName, 1024);

    puts(hostName);

    return EXIT_SUCCESS;

}

static bool keepRunning = true;

void intHandler(int)
{
    keepRunning = false;
    cout<<"HGHKVGH"<<endl;
}


int main(int argc, char *argv[])
{
    //for the server, we only need to specify a port number
    if(argc != 2)
    {
        cerr << "Usage: port" << endl;
        exit(0);
    }
    //grab the port number
    int port = atoi(argv[1]);
    //buffer to send and receive messages with
    char msg[1500];

    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }
    //bind the socket to its local address
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddr,
                          sizeof(servAddr));
    if(bindStatus < 0)
    {
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }
    cout << "Waiting for a client to connect..." << endl;
    //listen for up to 5 requests at a time
    listen(serverSd, 5);
    //receive a request from client using accept
    //we need a new address to connect with the client
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //accept, create a new socket descriptor to
    //handle the new connection with client
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newSd < 0)
    {
        cerr << "Error accepting request from client!" << endl;
        exit(1);
    }
    cout << "Connected with client!" << endl;
    //lets keep track of the session time
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    //also keep track of the amount of data sent as well
    int bytesRead, bytesWritten = 0;
    string statusCode="";
    bool isAccepted=false,isMailFrom=false,isMailTo=false,isDataAvailabe=false;
    cout << "Awaiting client response..." << endl;
    string host="shuvoPC";
    char toAdress[100];
    string user,mailSubject;
    char sub[100];
    //call function for hostname
    findHostName();
    while(1)
    {
        //receive a message from the client (listen)

        memset(&msg, 0, sizeof(msg));//clear the buffer

        recv(newSd, (char*)&msg, sizeof(msg), 0);
        strcpy(sub,msg);
        char* isSubjectHere = strchr(sub, '#');
        if(isSubjectHere != NULL)
        {
            mailSubject = extractSubject(sub);
        }

        strtok(msg,"#");
        if(!strcmp(msg, "QUIT"))
        {
            cout << "Client has quit the session" << endl;
            break;
        }
        if(!strcmp(msg, "hello"))
        {
            statusCode="202";
            isAccepted=true;
        }
        else if(isAccepted==true && !strcmp(msg, hostName))
        {
            statusCode="200";
            isMailFrom=true;
            isAccepted=false;
        }
        else if(isMailFrom)
        {
            strcpy(toAdress,msg);
            char* isValid = strchr(msg, '@');
            user = strtok(msg,"@");
            bool isFileHere = fileChecker(msg);
            if(isValid==NULL)  statusCode="450";
            else if(isValid!=NULL && isFileHere)
            {
                //Requested mail action okay, completed
                statusCode="250";
            }
            else
            {
                //Requested action not taken: mailbox unavailable
                statusCode="550";
            }
            isMailFrom=false;
            isMailTo=true;
        }
        else if(isMailTo && !strcmp(msg, "DATA"))
        {
            statusCode="354";
            isDataAvailabe=true;
            isMailTo=false;
        }
        else if(isDataAvailabe)
        {
            //Data rcved
            statusCode="222";
            //show in terminal
            cout<<"FROM: "<<hostName<<endl;
            cout<<"TO: "<<user<<endl;
            cout<<"Subject: "<<mailSubject<<endl;
            cout<<msg;
            cout<<"Date: ";
            printTime();
            cout<<"######   Above data is written in file  ######"<<endl;
            isDataAvailabe=false;

            //write in file
            freopen(user.c_str(),"a",stdout);
            cout<<"FROM: "<<hostName<<endl;
            cout<<"TO: "<<user<<endl;
            cout<<"Subject: "<<mailSubject<<endl;
            cout<<msg;
            cout<<"Date: ";
            printTime();
            cout<<endl;
            fclose(stdout);
        }
        else
        {
            //Syntax error, command unrecognised
            statusCode="500";
        }

        cout << "Client: " << msg << endl;
        cout << ">";
        memset(&msg, 0, sizeof(msg)); //clear the buffer
        strcpy(msg, statusCode.c_str());
        send(newSd, (char*)&msg, strlen(msg), 0);
        memset(&msg, 0, sizeof(msg));//clear the buffer

    }
    //we need to close the socket descriptors after we're all done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);

    cout << "Connection closed..." << endl;
    return 0;
}
