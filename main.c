#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_PALABRA 100

// Definiciones de estructuras
struct Libro {
    int id;
    char titulo[100];
    char autor[100];
    char contenido[10000];
};

struct Nodo {
    char palabra[10000];
    int contador;
    struct Nodo *izquierda;
    struct Nodo *derecha;
};

typedef struct Nodo Nodo;

struct Palabra {
    char texto[10000];
    int frecuencia;
};

Nodo *crearNodo(const char *palabra) {
    Nodo *nuevoNodo = (Nodo *)malloc(sizeof(Nodo));
    if (nuevoNodo != NULL) {
        strcpy(nuevoNodo->palabra, palabra);
        nuevoNodo->contador = 1;
        nuevoNodo->izquierda = NULL;
        nuevoNodo->derecha = NULL;
    }
    return nuevoNodo;
}

void insertarNodo(Nodo **raiz, const char *palabra) {
    if ((*raiz) == NULL) {
        (*raiz) = crearNodo(palabra);
    } else {
        int comparacion = strcmp(palabra, (*raiz)->palabra);
        if (comparacion == 0) {
            (*raiz)->contador++;
        } else if (comparacion < 0) {
            insertarNodo(&((*raiz)->izquierda), palabra);
        } else {
            insertarNodo(&((*raiz)->derecha), palabra);
        }
    }
}

void mostrarPalabrasFrecuentes(Nodo *raiz, int frecuenciaMinima) {
    if (raiz != NULL) {
        mostrarPalabrasFrecuentes(raiz->izquierda, frecuenciaMinima);
        if (raiz->contador >= frecuenciaMinima) {
            printf("%s: %d\n", raiz->palabra, raiz->contador);
        }
        mostrarPalabrasFrecuentes(raiz->derecha, frecuenciaMinima);
    }
}

void liberarArbol(Nodo *raiz) {
    if (raiz != NULL) {
        liberarArbol(raiz->izquierda);
        liberarArbol(raiz->derecha);
        free(raiz);
    }
}


void cargarArchivo(struct Libro libros[], int *numLibros, const char *rutaArchivo) {
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return;
    }

    char linea[256];  // Se asume que ninguna línea tendrá más de 256 caracteres

    // Leer el título del archivo
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        perror("Error al leer el título del archivo");
        fclose(archivo);
        return;
    }

    // Buscar "The Project Gutenberg eBook of" y eliminarlo del título
    char *tituloInicio = strstr(linea, "The Project Gutenberg eBook of");
    if (tituloInicio) {
        strcpy(libros[*numLibros].titulo, tituloInicio + strlen("The Project Gutenberg eBook of"));
    } else {
        strcpy(libros[*numLibros].titulo, linea);
    }

    libros[*numLibros].titulo[strcspn(libros[*numLibros].titulo, "\n")] = 0; // Eliminar el carácter de nueva línea

    if (fgets(libros[*numLibros].autor, sizeof(libros[*numLibros].autor), archivo) == NULL) {
        perror("Error al leer el autor del archivo");
        fclose(archivo);
        return;
    }
    libros[*numLibros].autor[strcspn(libros[*numLibros].autor, "\n")] = 0; // Eliminar el carácter de nueva línea

    if (fgets(libros[*numLibros].contenido, sizeof(libros[*numLibros].contenido), archivo) == NULL) {
        perror("Error al leer el contenido del archivo");
        fclose(archivo);
        return;
    }
    libros[*numLibros].contenido[strcspn(libros[*numLibros].contenido, "\n")] = 0; // Eliminar el carácter de nueva línea

    // Buscar "The Project Gutenberg eBook of" y eliminarlo del contenido
    char *tituloInicioContenido = strstr(libros[*numLibros].contenido, "The Project Gutenberg eBook of");
    if (tituloInicioContenido) {
        char *tituloFinContenido = strchr(tituloInicioContenido, '\n');
        if (tituloFinContenido) {
            // Calcula la longitud del título
            int longitudTituloContenido = tituloFinContenido - tituloInicioContenido;

            // Asegúrate de que la longitud del título no exceda el límite
            if (longitudTituloContenido >= sizeof(libros[*numLibros].titulo)) {
                longitudTituloContenido = sizeof(libros[*numLibros].titulo) - 1;
            }

            // Copia el título corregido
            strncpy(libros[*numLibros].titulo, tituloInicioContenido, longitudTituloContenido);
            libros[*numLibros].titulo[longitudTituloContenido] = '\0';
        }
    }

    // Aumentar el contador de libros
    (*numLibros)++;

    fclose(archivo);
}



void cargarDocumentos(struct Libro libros[], int *numLibros, const char *ruta) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(ruta);
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Comprobar si es un archivo regular
            char rutaArchivo[512]; // Tamaño suficiente para la ruta completa
            snprintf(rutaArchivo, sizeof(rutaArchivo), "%s/%s", ruta, entry->d_name);
            FILE *archivo = fopen(rutaArchivo, "r");
            if (archivo == NULL) {
                perror("No se pudo abrir el archivo");
            } else {
                // Leer el contenido del archivo y almacenarlo en la estructura
                libros[*numLibros].id = *numLibros + 1; // ID autoincremental
                fgets(libros[*numLibros].titulo, sizeof(libros[*numLibros].titulo), archivo);
                libros[*numLibros].titulo[strcspn(libros[*numLibros].titulo, "\n")] = 0; // Eliminar el carácter de nueva línea
                fgets(libros[*numLibros].autor, sizeof(libros[*numLibros].autor), archivo);
                libros[*numLibros].autor[strcspn(libros[*numLibros].autor, "\n")] = 0; // Eliminar el carácter de nueva línea
                fgets(libros[*numLibros].contenido, sizeof(libros[*numLibros].contenido), archivo);
                libros[*numLibros].contenido[strcspn(libros[*numLibros].contenido, "\n")] = 0; // Eliminar el carácter de nueva línea
                // Aumentar el contador de libros
                (*numLibros)++;
                fclose(archivo);
            }
        }
    }
    closedir(dir);
}

void mostrarDocumentosOrdenados(struct Libro libros[], int numLibros) {
    printf("Lista de Libros:\n");
    printf("ID\tTítulo\tAutor\tPalabras\tCaracteres\n");

    // Ordenar los libros por título antes de mostrarlos
    for (int i = 0; i < numLibros - 1; i++) {
        for (int j = i + 1; j < numLibros; j++) {
            if (strcmp(libros[i].titulo, libros[j].titulo) > 0) {
                // Intercambiar los libros
                struct Libro temp = libros[i];
                libros[i] = libros[j];
                libros[j] = temp;
            }
        }
    }

    for (int i = 0; i < numLibros; i++) {
        // Contar las palabras y caracteres en el contenido del libro
        int numPalabras = 0;
        int numCaracteres = 0;
        const char *contenido = libros[i].contenido;

        // Contar palabras y caracteres en el contenido
        int dentroDePalabra = 0;
        for (int j = 0; contenido[j] != '\0'; j++) {
            if (isalpha(contenido[j])) {
                dentroDePalabra = 1;
            } else {
                if (dentroDePalabra) {
                    numPalabras++;
                    dentroDePalabra = 0;
                }
            }
            numCaracteres++;
        }
        if (dentroDePalabra) {
            numPalabras++;
        }

        printf("%d\t%s\t%s\t%d\t%d\n", i + 1, libros[i].titulo, libros[i].autor, numPalabras, numCaracteres);
    }
}

void contarPalabra(struct Palabra palabras[], int *numPalabras, const char *texto) {
    if (*numPalabras < 100) { // Verificar si el arreglo no está lleno
        for (int i = 0; i < *numPalabras; i++) {
            if (strcmp(palabras[i].texto, texto) == 0) {
                palabras[i].frecuencia++;
                return;
            }
        }

        // La palabra no se ha encontrado, agrégala a la lista
        strcpy(palabras[*numPalabras].texto, texto);
        palabras[*numPalabras].frecuencia = 1;
        (*numPalabras)++;
    }
}

void palabrasMasFrecuentes(struct Libro libros[], int numLibros, int idLibro) {
    if (idLibro < 1 || idLibro > numLibros) {
        printf("ID de libro no válido.\n");
        return;
    }

    // Obtén el libro con el ID especificado
    struct Libro libro = libros[idLibro - 1];

    struct Palabra palabras[100]; // Ajusta el tamaño del arreglo según tus necesidades
    int numPalabras = 0;

    char *token = strtok(libro.contenido, " ");
    Nodo *raiz = NULL; // Árbol para palabras (se declara aquí)

    while (token != NULL) {
        contarPalabra(palabras, &numPalabras, token);
        insertarNodo(&raiz, token);  // Insertar palabra en el árbol
        token = strtok(NULL, " ");
    }

    // Ordena la lista de palabras por frecuencia (mayor a menor)
    for (int i = 0; i < numPalabras - 1; i++) {
        for (int j = i + 1; j < numPalabras; j++) {
            if (palabras[i].frecuencia < palabras[j].frecuencia) {
                // Intercambia las palabras
                struct Palabra temp = palabras[i];
                palabras[i] = palabras[j];
                palabras[j] = temp;
            }
        }
    }

    // Muestra las palabras más frecuentes
    printf("Palabras más frecuentes en el libro:\n");
    for (int i = 0; i < numPalabras; i++) {
        printf("%s: %d veces\n", palabras[i].texto, palabras[i].frecuencia);
    }

    // Muestra palabras más frecuentes utilizando el árbol
    printf("Palabras más frecuentes en el libro (utilizando el árbol):\n");
    mostrarPalabrasFrecuentes(raiz, 1);

    // Libera la memoria del árbol
    liberarArbol(raiz);
}

void palabrasMasRelevantes(struct Libro libros[], int numLibros, char tituloLibro[]) {
    int i;
    for (i = 0; i < numLibros; i++) {
        if (strcmp(libros[i].titulo, tituloLibro) == 0) {
            break;
        }
    }

    if (i == numLibros) {
        printf("Libro no encontrado.\n");
        return;
    }

    // Obtén el libro con el título especificado
    struct Libro libro = libros[i];

    struct Palabra palabras[100]; // Ajusta el tamaño del arreglo según tus necesidades
    int numPalabras = 0;
    char *token = strtok(libro.contenido, " ");
    Nodo *raiz = NULL; // Árbol para palabras (se declara aquí)

    while (token != NULL) {
        contarPalabra(palabras, &numPalabras, token);
        insertarNodo(&raiz, token);  // Insertar palabra en el árbol
        token = strtok(NULL, " ");
    }

    // En este ejemplo simplificado, simplemente muestra todas las palabras y sus frecuencias
    printf("Palabras más relevantes en el libro:\n");
    for (int i = 0; i < numPalabras; i++) {
        printf("%s: %d veces\n", palabras[i].texto, palabras[i].frecuencia);
    }

    // Muestra palabras más frecuentes utilizando el árbol
    printf("Palabras más relevantes en el libro (utilizando el árbol):\n");
    mostrarPalabrasFrecuentes(raiz, 1);

    // Libera la memoria del árbol
    liberarArbol(raiz);
}

void buscarPorPalabra(struct Libro libros[], int numLibros, char palabraBuscada[]) {
    printf("Resultados de la búsqueda por palabra clave '%s':\n", palabraBuscada);
    printf("ID\tTítulo\tAutor\n");

    int encontrados = 0;
    for (int i = 0; i < numLibros; i++) {
        // Buscar la palabra clave en el contenido del libro
        if (strstr(libros[i].contenido, palabraBuscada) != NULL) {
            printf("%d\t%s\t%s\n", libros[i].id, libros[i].titulo, libros[i].autor);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Ningún libro contiene la palabra clave.\n");
    }
}

void mostrarPalabraEnContexto(struct Libro libros[], int numLibros, char tituloLibro[], char palabraBuscada[]) {
    int i;
    for (i = 0; i < numLibros; i++) {
        if (strcmp(libros[i].titulo, tituloLibro) == 0) {
            break;
        }
    }

    if (i == numLibros) {
        printf("Libro no encontrado.\n");
        return;
    }

    // Obtener el libro con el título especificado
    struct Libro libro = libros[i];

    // Buscar la palabra en el contenido y mostrar el contexto
    char *contenido = libro.contenido;
    char *pos = strstr(contenido, palabraBuscada);

    while (pos != NULL) {
        int posicion = pos - contenido;
        int inicio = (posicion - 5 < 0) ? 0 : posicion - 5;
        int fin = (posicion + strlen(palabraBuscada) + 5 > strlen(contenido)) ? strlen(contenido) : posicion + strlen(palabraBuscada) + 5;

        for (int j = inicio; j < fin; j++) {
            printf("%c", contenido[j]);
        }
        printf("\n");

        pos = strstr(pos + 1, palabraBuscada);
        }
        }

       int main() {
    struct Libro libros[100];
    int numLibros = 0;
    int opcion;
    char tituloLibro[100];
    char palabraBuscada[100];
    Nodo *raiz = NULL;

    do {
        printf("\n |---------------------------------------------------|\n");
        printf(" |                 *Menú de Búsqueda*                |\n");
        printf(" |---------------------------------------------------|\n");
        printf("1. Cargar documentos\n");
        printf("2. Mostrar documentos ordenados por título\n");
        printf("3. Buscar libro por título\n");
        printf("4. Palabras con mayor frecuencia en un libro\n");
        printf("5. Palabras más relevantes en un libro\n");
        printf("6. Buscar libros por palabra clave\n");
        printf("7. Mostrar palabra en su contexto dentro de un libro\n");
        printf("8. Salir\n\n");
        printf("-----------------------------------------------------\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                int tipoArchivo;
                printf("Seleccione el tipo de archivo a cargar:\n");
                printf("1. Archivo\n");
                printf("2. Carpeta\n");
                printf("Ingrese su elección: ");
                scanf("%d", &tipoArchivo);
                if (tipoArchivo == 1) {
                    char rutaArchivo[512];
                    printf("Ingrese el nombre del archivo: ");
                    scanf("%s", rutaArchivo);
                    cargarArchivo(libros, &numLibros, rutaArchivo);
                } else if (tipoArchivo == 2) {
                    char rutaCarpeta[512];
                    printf("Ingrese el nombre de la carpeta: ");
                    scanf("%s", rutaCarpeta);
                    cargarDocumentos(libros, &numLibros, rutaCarpeta);
                } else {
                    printf("Opción no válida.\n");
                }
                break;
            }
            case 2:
                mostrarDocumentosOrdenados(libros, numLibros);
                break;
            case 3:
                printf("Ingrese el título del libro a buscar: ");
                getchar();
                fgets(tituloLibro, sizeof(tituloLibro), stdin);
                tituloLibro[strcspn(tituloLibro, "\n")] = 0;
                palabrasMasRelevantes(libros, numLibros, tituloLibro);
                break;
            case 4: {
                int idLibro;
                printf("Ingrese el ID del libro para encontrar las palabras más frecuentes: ");
                scanf("%d", &idLibro);
                palabrasMasFrecuentes(libros, numLibros, idLibro);
                break;
            }
            case 5: {
                printf("Ingrese el título del libro a analizar: ");
                getchar();
                fgets(tituloLibro, sizeof(tituloLibro), stdin);
                tituloLibro[strcspn(tituloLibro, "\n")] = 0;
                palabrasMasRelevantes(libros, numLibros, tituloLibro);
                break;
            }
            case 6:
                printf("Ingrese la palabra clave para buscar libros: ");
                getchar();
                fgets(palabraBuscada, sizeof(palabraBuscada), stdin);
                palabraBuscada[strcspn(palabraBuscada, "\n")] = 0;
                buscarPorPalabra(libros, numLibros, palabraBuscada);
                break;
            case 7:
                printf("Ingrese el título del libro en el que desea buscar: ");
                getchar();
                fgets(tituloLibro, sizeof(tituloLibro), stdin);
                tituloLibro[strcspn(tituloLibro, "\n")] = 0;
                printf("Ingrese la palabra que desea buscar: ");
                fgets(palabraBuscada, sizeof(palabraBuscada), stdin);
                palabraBuscada[strcspn(palabraBuscada, "\n")] = 0;
                mostrarPalabraEnContexto(libros, numLibros, tituloLibro, palabraBuscada);
                break;
            case 8:
                printf("Saliendo del programa.\n");
                break;
            default:
                printf("Opción no válida.\n");
                break;
        }
    } while (opcion != 8);

    liberarArbol(raiz);

    return 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
