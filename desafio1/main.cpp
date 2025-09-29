#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <cstdio>

using namespace std;

size_t miStrlen(const char *s);
void miStrcpy(char *dest, const char *src);
void miMemcpy(void *dest, const void *src, size_t n);

unsigned char* limpiarBasura(const unsigned char* entrada, size_t longitud, size_t &nuevoTam);
void limpiarPista(char *pista);
bool esImprimible(unsigned char b);
void mostrarMuestra(const unsigned char *buf, size_t len);

char **agregarEntrada(char **diccionario, int &tamano, const char *nuevaEntrada);
char *LZ78_Decode(const unsigned char *input, size_t longitud);
char *RLE_Decode(const unsigned char *entrada, size_t longitud);

unsigned char rotarDerecha(unsigned char valor, int bits);
void aplicarXOR(unsigned char *datos, size_t longitud, unsigned char clave);
unsigned char *decodificadorXorYRotacion(const unsigned char *entrada, size_t longitud,unsigned char claveXor, int bitsRotacion);
bool tienePista(const char *texto, size_t lenTexto, const char *pista);

int main() {
    int cantidadArchivos;
    cout << "Ingrese la cantidad de archivos a evaluar: ";
    cin >> cantidadArchivos;

    for (int caso = 1; caso <= cantidadArchivos; ++caso) {
        char nombreEncriptado[256], nombrePista[256];
        sprintf(nombreEncriptado, "C:\\Users\\USUARIO\\desafio1\\desafio1\\Encriptado%d.txt", caso);
        sprintf(nombrePista,      "C:\\Users\\USUARIO\\desafio1\\desafio1\\pista%d.txt",     caso);

        ifstream archivoEncriptado(nombreEncriptado, ios::binary);
        ifstream archivoPista(nombrePista);
        if (!archivoEncriptado || !archivoPista) {
            cerr << "Error abriendo los archivos del caso " << caso << "\n";
            continue;
        }

        char pista[1024];
        archivoPista.getline(pista, 1024);
        limpiarPista(pista);

        archivoEncriptado.seekg(0, ios::end);
        size_t tam = (size_t)archivoEncriptado.tellg();
        archivoEncriptado.seekg(0, ios::beg);

        unsigned char *mensaje = new unsigned char[tam + 1];
        archivoEncriptado.read((char*)mensaje, tam);
        archivoEncriptado.close();
        archivoPista.close();

        cout << "\n=== Caso " << caso << " ===\n";
        cout << "Archivo: " << nombreEncriptado << "  tamaño: " << tam << " bytes\n";
        cout << "Pista (limpia): '" << pista << "' (len=" << miStrlen(pista) << ")\n\n";

        bool encontrado = false;
        unsigned char claveCorrecta = 0;
        int rotacionCorrecta = 0;
        const char *metodoCompresion = NULL;
        char *textoOriginal = NULL;

        for (int rot = 1; rot <= 7 && !encontrado; ++rot) {
            for (int clave = 0; clave <= 255 && !encontrado; ++clave) {
                unsigned char *des = decodificadorXorYRotacion(mensaje, tam, (unsigned char)clave, rot);
                size_t limpioTam = 0;
                unsigned char *limpio = limpiarBasura(des, tam, limpioTam);

                char *rle = RLE_Decode(limpio, limpioTam);
                if (rle && miStrlen(rle) > 0) {
                    if (tienePista(rle, miStrlen(rle), pista)) {
                        metodoCompresion = "RLE";
                        claveCorrecta = (unsigned char)clave;
                        rotacionCorrecta = rot;
                        textoOriginal = rle;
                        encontrado = true;
                        delete [] des;
                        delete [] limpio;
                        break;
                    }
                    delete [] rle;
                }

                char *lz = LZ78_Decode(limpio, limpioTam);
                if (lz && miStrlen(lz) > 0) {
                    if (tienePista(lz, miStrlen(lz), pista)) {
                        metodoCompresion = "LZ78";
                        claveCorrecta = (unsigned char)clave;
                        rotacionCorrecta = rot;
                        textoOriginal = lz;
                        encontrado = true;
                        delete [] des;
                        delete [] limpio;
                        break;
                    }
                    delete [] lz;
                }

                if (!encontrado) {
                    if (tienePista((const char*)limpio, limpioTam, pista)) {
                        metodoCompresion = "Directo";
                        claveCorrecta = (unsigned char)clave;
                        rotacionCorrecta = rot;
                        textoOriginal = new char[limpioTam + 1];
                        miMemcpy(textoOriginal, limpio, limpioTam);
                        textoOriginal[limpioTam] = '\0';
                        encontrado = true;
                        delete [] des;
                        delete [] limpio;
                        break;
                    }
                }

                delete [] des;
                delete [] limpio;
            }
        }

        if (encontrado) {
            cout << "[✓] Caso " << caso << " resuelto.\n";
            cout << "Método: " << metodoCompresion << "\n";
            cout << "Clave XOR: 0x" << hex << (int)claveCorrecta << dec << "\n";
            cout << "Rotación: " << rotacionCorrecta << "\n";
            cout << "Texto:\n" << textoOriginal << "\n";
            delete [] textoOriginal;
        } else {
            cout << "No se encontró combinación válida.\n";
        }

        delete [] mensaje;
    }
    return 0;
}

size_t miStrlen(const char *s) {
    size_t n = 0;
    while (s[n] != '\0') n++;
    return n;
}

void miStrcpy(char *dest, const char *src) {
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

void miMemcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
}

// --- funciones de limpieza ---
unsigned char* limpiarBasura(const unsigned char* entrada, size_t longitud, size_t &nuevoTam) {
    // NUEVO: elimina todo lo que NO sea 0-9 A-Z a-z
    unsigned char* limpio = new unsigned char[longitud + 1];
    size_t j = 0;
    for (size_t i = 0; i < longitud; ++i) {
        unsigned char c = entrada[i];
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z')) {
            limpio[j++] = c;
        }
    }
    limpio[j] = '\0';
    nuevoTam = j;
    return limpio;
}

void limpiarPista(char *pista) {
    int len = (int)miStrlen(pista);
    int j = 0;
    for (int i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)pista[i];
        if (c != ' ' && c != '\r' && c != '\n' && c != '\t') pista[j++] = pista[i];
    }
    pista[j] = '\0';
}

bool esImprimible(unsigned char b) {
    return (b >= 32 && b <= 126) || b == '\n' || b == '\r' || b == '\t';
}

void mostrarMuestra(const unsigned char *buf, size_t len) {
    size_t m = len < 64 ? len : 64;
    for (size_t i = 0; i < m; ++i) {
        unsigned char c = buf[i];
        if (esImprimible(c)) cout << (char)c;
        else cout << '.';
    }
    if (len > m) cout << "...";
    cout << "\nHEX: ";
    size_t mh = len < 16 ? len : 16;
    for (size_t i = 0; i < mh; ++i) printf("%02X ", buf[i]);
    if (len > mh) cout << "...";
    cout << "\n";
}

char **agregarEntrada(char **diccionario, int &tamano, const char *nuevaEntrada) {
    char **temporal = new char *[tamano + 1];
    for (int i = 0; i < tamano; ++i) temporal[i] = diccionario[i];
    size_t len = miStrlen(nuevaEntrada);
    temporal[tamano] = new char[len + 1];
    miStrcpy(temporal[tamano], nuevaEntrada);
    delete [] diccionario;
    tamano++;
    return temporal;
}

char *LZ78_Decode(const unsigned char *input, size_t longitud) {
    char **dictionary = nullptr;
    int dictSize = 0;
    char *result = new char[1]; result[0] = '\0';
    int resultLen = 0;

    const unsigned char *p = input;
    const unsigned char *end = input + longitud;

    while (p < end) {
        if (*p == '(') {
            ++p;
            char numeroBuffer[32]; int nb = 0;
            while (p < end && *p >= '0' && *p <= '9') numeroBuffer[nb++] = (char)*p++;
            numeroBuffer[nb] = '\0';
            int index = atoi(numeroBuffer);

            if (p < end && *p == ',') ++p;
            if (p >= end) break;
            char c = (char)*p++;
            if (p < end && *p == ')') ++p;

            int prefixLen = 0;
            const char *prefix = "";
            if (index > 0 && index <= dictSize) {
                prefix = dictionary[index-1];
                prefixLen = miStrlen(prefix);
            }
            char *entry = new char[prefixLen + 2];
            if (prefixLen) miStrcpy(entry, prefix);
            entry[prefixLen] = c;
            entry[prefixLen + 1] = '\0';

            dictionary = agregarEntrada(dictionary, dictSize, entry);
            delete [] entry;

            int eLen = miStrlen(dictionary[dictSize-1]);
            char *tmpRes = new char[resultLen + eLen + 1];
            miStrcpy(tmpRes, result);
            miStrcpy(tmpRes + resultLen, dictionary[dictSize-1]);
            delete [] result;
            result = tmpRes;
            resultLen += eLen;
        } else ++p;
    }

    for (int i = 0; i < dictSize; ++i) delete [] dictionary[i];
    delete [] dictionary;
    return result;
}

char *RLE_Decode(const unsigned char *entrada, size_t longitud) {
    size_t i = 0;
    char *salida = new char[1]; salida[0] = '\0';
    int longitudSalida = 0;

    while (i < longitud) {
        int ib = 0;
        char bufNum[64];
        while (i < longitud && entrada[i] >= '0' && entrada[i] <= '9') bufNum[ib++] = (char)entrada[i++];
        bufNum[ib] = '\0';
        if (ib == 0) {
            delete [] salida;
            char *v = new char[1]; v[0] = '\0';
            return v;
        }
        int cantidad = atoi(bufNum);
        if (i >= longitud) break;
        char caracter = (char)entrada[i++];
        char *tmp = new char[longitudSalida + cantidad + 1];
        miStrcpy(tmp, salida);
        for (int k = 0; k < cantidad; ++k) tmp[longitudSalida + k] = caracter;
        tmp[longitudSalida + cantidad] = '\0';
        delete [] salida;
        salida = tmp;
        longitudSalida += cantidad;
    }
    return salida;
}

unsigned char rotarDerecha(unsigned char valor, int bits) {
    bits &= 7;
    return (unsigned char)((valor >> bits) | (valor << (8 - bits)));
}

void aplicarXOR(unsigned char *datos, size_t longitud, unsigned char clave) {
    for (size_t i = 0; i < longitud; ++i) datos[i] ^= clave;
}

unsigned char *decodificadorXorYRotacion(const unsigned char *entrada, size_t longitud,
                                         unsigned char claveXor, int bitsRotacion) {
    unsigned char *buffer = new unsigned char[longitud];
    miMemcpy(buffer, entrada, longitud);
    aplicarXOR(buffer, longitud, claveXor);
    for (size_t i = 0; i < longitud; ++i) buffer[i] = rotarDerecha(buffer[i], bitsRotacion);
    return buffer;
}

bool tienePista(const char *texto, size_t lenTexto, const char *pista) {
    size_t longPista = miStrlen(pista);
    if (longPista == 0 || lenTexto < longPista) return false;
    for (size_t i = 0; i + longPista <= lenTexto; ++i) {
        size_t j = 0;
        while (j < longPista && texto[i + j] == pista[j]) ++j;
        if (j == longPista) return true;
    }
    return false;
}
