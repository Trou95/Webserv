#include "Server.h"
#include "Parser/cgiparser/CgiParser.h"

void print(stScope server)
{
    cout << "Name: " << server.name << endl;
    cout << "Args: ";
    for(auto p : server.args)
        cout << p << " ";
    cout << endl;
    cout << " Values: " << endl;
    for(auto p : server.values)
    {
        cout << " " << p.first << " ";
        for(auto p2 : p.second)
            cout << " " << p2 << " ";
        cout << endl;
    }
    for(int i = 0; i < server.scopes.size(); i++)
        print(server.scopes[i]);
}

int main()
{
    //Server server;
    //server.Run();

    string res = FileReader::readFile("default.conf");
    CgiParser parser;
    vector<stScope> tmp = parser.Parse(res);

    for(int i = 0; i < tmp.size(); i++)
    {
       print(tmp[i]);
    }

}