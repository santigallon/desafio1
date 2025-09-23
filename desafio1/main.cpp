#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;
char** addEntry(char  **diccionario, int&tamano,const char *newEntry);
char* LZ78Decode(const char *ingresso);

int main()
{

}
char** addEntry(char  **diccionario, int&tamano,const char *newEntry)
{
    char ** temporal = new char* [tamano +1];
    for (int i=0; i<tamano;i++) temporal [i]=diccionario[i];
    temporal[tamano] =new char [strlen (newEntry) + 1];
    strcpy(temporal [tamano],newEntry);
    delete []diccionario;
    tamano++;
    return temporal;
}
char* LZ78Decode(const char *innt)
{
    char **diccionario = nullptr;
    int tamDiccionario=0;
    char *resul = new char[1]
        resul[0]='\0';
    int resultado=0;
    const char *p =innt;
    while(*P)
    {
        if (*p =='(')
        {
            p++;
char numbuf
        }
    }
}
