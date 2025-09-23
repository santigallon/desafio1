#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;
char** addEntry(char  **diccionario, int&tamano,const char *newEntry);
char* LZ78_Decode(const char *ingresso);

int main()
{
    const char *entrada = "(0,a)(0,b)(1,b)(3,a)";
    char *salida = LZ78_Decode(entrada);
    cout << "Texto decodificado: " << salida << "\n";
    delete [] salida;
    return 0;
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
char* LZ78_Decode(const char *input) {
    char **dictionary = nullptr;
    int dictSize = 0;

    // buffer para el resultado final (lo vamos agrandando)
    char *result = new char[1];
    result[0] = '\0';
    int resultLen = 0;

    const char *p = input;
    while (*p) {
        if (*p == '(') {
            p++;
            // leer índice
            char numBuf[16]; int nb = 0;
            while (*p >= '0' && *p <= '9') numBuf[nb++] = *p++;
            numBuf[nb] = '\0';
            int index = atoi(numBuf);

            if (*p == ',') p++;  // saltar coma
            char c = *p;         // caracter
            p++; // mover después del caracter
            if (*p == ')') p++;

            // construir nueva cadena = dic[index-1] + c (si index!=0)
            int prefixLen = 0;
            const char *prefix = "";
            if (index > 0 && index <= dictSize) {
                prefix = dictionary[index-1];
                prefixLen = strlen(prefix);
            }
            char *entry = new char[prefixLen + 2]; // +1 c +1 '\0'
            if (prefixLen) strcpy(entry, prefix);
            entry[prefixLen] = c;
            entry[prefixLen + 1] = '\0';

            // añadir entry al diccionario
            dictionary = addEntry(dictionary, dictSize, entry);
            delete [] entry; // addEntry ya hizo su propia copia

            // añadir entry al resultado
            int eLen = strlen(dictionary[dictSize-1]);
            char *tmpRes = new char[resultLen + eLen + 1];
            strcpy(tmpRes, result);
            strcpy(tmpRes + resultLen, dictionary[dictSize-1]);
            delete [] result;
            result = tmpRes;
            resultLen += eLen;
        } else {
            p++;
        }
    }

    for (int i = 0; i < dictSize; i++) delete [] dictionary[i];
    delete [] dictionary;

    return result;
}
