#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;
char **agregarEntrada(char  **diccionario, int&tamano,const char *nuevaEntrada);
char *LZ78_Decode(const char* ingresso);
char *RLE_Decode(const char* entrada);
unsigned char rotarDerecha(unsigned char valor, int bits);
void aplicarXOR(unsigned char *datos, size_t longitud, unsigned char clave);
char *decodificadorXorYRotacion (const unsigned char *entrada, size_t longitud, unsigned char claveXor, int bitsRotacion);
bool tienePista(const char *texto, const char *pista);

int main()
{

}

char **agregarEntrada(char  **diccionario, int&tamano,const char *nuevaEntrada)
{
    char **temporal = new char *[tamano +1];
    for (int i=0; i<tamano;i++)
    {
        temporal [i]=diccionario[i];
    }
    temporal[tamano] =new char [strlen (nuevaEntrada) + 1];
    strcpy(temporal [tamano],nuevaEntrada );

    delete []diccionario;

    tamano++;

    return temporal;
}

char *LZ78_Decode(const char *input)
{
    char **dictionary = nullptr;
    int dictSize = 0;


    char *result = new char[1];
    result[0] = '\0';
    int resultLen = 0;

    const char *p = input;
    while (*p)
    {
        if (*p == '(') {
            p++;
            char numeroBuffer[16]; int nb = 0;
            while (*p >= '0' && *p <= '9') numeroBuffer[nb++] = *p++;
            numeroBuffer[nb] = '\0';
            int index = atoi(numeroBuffer);

            if (*p == ',') p++;
            char c = *p;
            p++;
            if (*p == ')') p++;

            int prefixLen = 0;
            const char *prefix = "";
            if (index > 0 && index <= dictSize)
            {
                prefix = dictionary[index-1];
                prefixLen = strlen(prefix);
            }
            char *entry = new char[prefixLen + 2];
            if (prefixLen) strcpy(entry, prefix);
            entry[prefixLen] = c;
            entry[prefixLen + 1] = '\0';

            dictionary = agregarEntrada(dictionary, dictSize, entry);
            delete [] entry;

            int eLen = strlen(dictionary[dictSize-1]);
            char *tmpRes = new char[resultLen + eLen + 1];
            strcpy(tmpRes, result);
            strcpy(tmpRes + resultLen, dictionary[dictSize-1]);
            delete [] result;
            result = tmpRes;
            resultLen += eLen;
        } else
        {
            p++;
        }
    }

    for (int i = 0; i < dictSize; i++) delete [] dictionary[i];
    delete [] dictionary;

    return result;
}

char *RLE_Decode(const char *entrada)
{
    const char *p = entrada;
    char *salida = new char[1];
    salida[0]='\0';
    int longitudSalida=0;

    while(*p)
    {
        int indiceBuffer = 0;
        char bufferNumero[32];
        while(*p >= '0' && *p <= '9')
        {
            bufferNumero[indiceBuffer++]=*p;
            p++;
        }
        bufferNumero[indiceBuffer]='\0';
        int cantidad = atoi (bufferNumero);
        if (*p =='\0')break;

        char caracter =*p;
        p++;
        char *temporal = new char[longitudSalida +cantidad + 1];
        strcpy(temporal, salida);
        for (int i=0;i<cantidad;i++)
        {
            temporal[longitudSalida + i] = caracter;
        }
        temporal[longitudSalida + cantidad] = '\0';

        delete [] salida;
        salida = temporal;
        longitudSalida += cantidad;
    }
    return salida;
}

unsigned char rotarDerecha(unsigned char valor, int bits)
{
    bits &=7;
    return(unsigned char)((valor >> bits)| (valor << (8 - bits)));
}

void aplicarXOR(unsigned char *datos, size_t longitud, unsigned char clave)
{
    for (size_t i = 0; i < longitud; i++)
    {
        datos[i]^=clave;
    }
}

char *decodificadorXorYRotacion (const unsigned char *entrada, size_t longitud, unsigned char claveXor, int bitsRotacion)
{
    unsigned char *buffer = new unsigned char [longitud];
    memcpy(buffer, entrada, longitud);

    aplicarXOR(buffer, longitud, claveXor);

    for(size_t i = 0; i <= longitud; i++)
    {
        buffer[i] = rotarDerecha(buffer[i], bitsRotacion);
    }
    char *salida = new char[longitud + 1];
    memcpy(salida, buffer, longitud);
    salida [longitud] = '\0';
    delete [] buffer;
    return salida;
}

bool tienePista(const char *texto, const char *pista)
{
    size_t longTexto = strlen(texto);
    size_t longPista = strlen(pista);
    for (size_t i = 0 ; i + longPista <= longTexto ; i++)
    {
        size_t j=0;
        while(j < longPista && texto[i + j] == pista[j])
        {
            j++;
        }
        if (j == longPista)
        {
            return true;
        }

    }
    return false;
}
