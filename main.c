//*****************************************************************
// CREDITOS POR USO DE CODIGO:
// EDGARDO ADRIÁN FRANCO MARTÍNEZ
//(C) Agosto 2010 Versión 1.5
// Lectura, escritura y tratamiento de imagenes BMP
// Compilación: "gcc BMP.c -o BMP"
// Ejecución: "./BMP imagen.bmp"
// Observaciones "imagen.bmp" es un BMP de 24 bits

// Archivo modificado por Mariela Curiel par leer toda la imagen en la memoria
// y hacer la conversion de los pixeles en una funci'on. Esto facilita la
// programacion posterior con hilos.

//*****************************************************************
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct BMP {
  char bm[2];     //(2 Bytes) BM (Tipo de archivo)
  int tamano;     //(4 Bytes) Tamaño del archivo en bytes
  int reservado;  //(4 Bytes) Reservado
  int offset;  //(4 Bytes) offset, distancia en bytes entre la img y los píxeles
  int tamanoMetadatos;     //(4 Bytes) Tamaño de Metadatos (tamaño de esta
                           // estructura = 40)
  int alto;                //(4 Bytes) Ancho (numero de píxeles horizontales)
  int ancho;               //(4 Bytes) Alto (numero de pixeles verticales)
  short int numeroPlanos;  //(2 Bytes) Numero de planos de color
  short int
      profundidadColor;  //(2 Bytes) Profundidad de color (debe ser 24 para
                         // nuestro caso)
  int tipoCompresion;  //(4 Bytes) Tipo de compresión (Vale 0, ya que el bmp es
                       // descomprimido)
  int tamanoEstructura;    //(4 Bytes) Tamaño de la estructura Imagen (Paleta)
  int pxmh;                //(4 Bytes) Píxeles por metro horizontal
  int pxmv;                //(4 Bytes) Píxeles por metro vertical
  int coloresUsados;       //(4 Bytes) Cantidad de colores usados
  int coloresImportantes;  //(4 Bytes) Cantidad de colores importantes
  unsigned char ***pixel;  // Puntero a una tabla dinamica de caracteres de 3
                           // dimensiones para almacenar los pixeles
} BMP;
typedef struct {
  BMP *imagen;
  int nhilos;
  int ini;
  int fin;
} argumentos_t;
void abrir_imagen(BMP *imagen, char *ruta) {
  FILE *archivo;
  int i, j, k;
  unsigned char P[3];
  // Abrir el archivo de imágen
  archivo = fopen(ruta, "rb+");
  if (!archivo) {
    // Si la imágen no se encuentra en la ruta dada
    printf("La imágen %s no se encontro\n", ruta);
    exit(1);
  }
  // Leer la cabecera de la imagen y almacenarla en la estructura a la que
  // apunta imagen
  fseek(archivo, 0, SEEK_SET);
  fread(&imagen->bm, sizeof(char), 2, archivo);
  fread(&imagen->tamano, sizeof(int), 1, archivo);
  fread(&imagen->reservado, sizeof(int), 1, archivo);
  fread(&imagen->offset, sizeof(int), 1, archivo);
  fread(&imagen->tamanoMetadatos, sizeof(int), 1, archivo);
  fread(&imagen->alto, sizeof(int), 1, archivo);
  fread(&imagen->ancho, sizeof(int), 1, archivo);
  fread(&imagen->numeroPlanos, sizeof(short int), 1, archivo);
  fread(&imagen->profundidadColor, sizeof(short int), 1, archivo);
  fread(&imagen->tipoCompresion, sizeof(int), 1, archivo);
  fread(&imagen->tamanoEstructura, sizeof(int), 1, archivo);
  fread(&imagen->pxmh, sizeof(int), 1, archivo);
  fread(&imagen->pxmv, sizeof(int), 1, archivo);
  fread(&imagen->coloresUsados, sizeof(int), 1, archivo);
  fread(&imagen->coloresImportantes, sizeof(int), 1, archivo);
  // Validar ciertos datos de la cabecera de la imágen
  if (imagen->bm[0] != 'B' || imagen->bm[1] != 'M') {
    printf("La imagen debe ser un bitmap.\n");
    exit(1);
  }
  if (imagen->profundidadColor != 24) {
    printf("La imagen debe ser de 24 bits.\n");
    exit(1);
  }
  // Reservar memoria para la matriz de pixels
  imagen->pixel = malloc(imagen->alto * sizeof(char *));
  for (i = 0; i < imagen->alto; i++) {
    imagen->pixel[i] = malloc(imagen->ancho * sizeof(char *));
  }
  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++)
      imagen->pixel[i][j] = malloc(3 * sizeof(char));
  }
  // Pasar la imágen a el arreglo reservado en escala de grises
  // unsigned char R,B,G;
  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      for (k = 0; k < 3; k++) {
        fread(&P[k], sizeof(char), 1, archivo);        // Byte Blue del pixel
        imagen->pixel[i][j][k] = (unsigned char)P[k];  // Formula correcta
      }
    }
  }
  // Cerrrar el archivo
  fclose(archivo);
}
void crear_imagen(BMP *imagen, char ruta[]) {
  FILE *archivo;  // Puntero FILE para el archivo de imágen a abri
  int i, j, k;
  // Abrir el archivo de imágen
  archivo = fopen(ruta, "wb+");
  if (!archivo) {
    // Si la imágen no se encuentra en la ruta dada
    printf("La imágen %s no se pudo crear\n", ruta);
    exit(1);
  }
  // Escribir la cabecera de la imagen en el archivo
  fseek(archivo, 0, SEEK_SET);
  fwrite(&imagen->bm, sizeof(char), 2, archivo);
  fwrite(&imagen->tamano, sizeof(int), 1, archivo);
  fwrite(&imagen->reservado, sizeof(int), 1, archivo);
  fwrite(&imagen->offset, sizeof(int), 1, archivo);
  fwrite(&imagen->tamanoMetadatos, sizeof(int), 1, archivo);
  fwrite(&imagen->alto, sizeof(int), 1, archivo);
  fwrite(&imagen->ancho, sizeof(int), 1, archivo);
  fwrite(&imagen->numeroPlanos, sizeof(short int), 1, archivo);
  fwrite(&imagen->profundidadColor, sizeof(short int), 1, archivo);
  fwrite(&imagen->tipoCompresion, sizeof(int), 1, archivo);
  fwrite(&imagen->tamanoEstructura, sizeof(int), 1, archivo);
  fwrite(&imagen->pxmh, sizeof(int), 1, archivo);
  fwrite(&imagen->pxmv, sizeof(int), 1, archivo);
  fwrite(&imagen->coloresUsados, sizeof(int), 1, archivo);
  fwrite(&imagen->coloresImportantes, sizeof(int), 1, archivo);
  // Pasar la imágen del arreglo reservado en escala de grises a el archivo
  // (Deben escribirse los valores BGR)
  for (i = 0; i < imagen->alto; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      for (k = 0; k < 3; k++)
        fwrite(&imagen->pixel[i][j][k], sizeof(char), 1,
               archivo);  // Escribir el Byte Blue del pixel
    }
  }
  // Cerrrar el archivo
  fclose(archivo);
}

void *primer(void *args) {  // FUNCION PARA LA PRIMERA OPCION LLAMADA POR HILOS
  argumentos_t *arg = (argumentos_t *)args;
  int j, k, i;
  unsigned char temp;
  BMP *imagen = arg->imagen;
  int nhilos = arg->nhilos;
  int inicial = arg->ini;
  int finale = arg->fin;  // SE ASIGNA VARIABLES EXTRAIDAS DEL OBJETO CREADO
  for (i = inicial; i <= finale; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      temp = (unsigned char)((imagen->pixel[i][j][2] * 0.3) +
                             (imagen->pixel[i][j][1] * 0.59) +
                             (imagen->pixel[i][j][0] *
                              0.11));  // SE BUSCA EL NUMERO QUE SE VA A PONER
      for (k = 0; k < 3; k++) {
        imagen->pixel[i][j][k] = (unsigned char)temp;  // Formula correcta
      }
    }
  }
}
void *segundo(void *args) {  // FUNCION PARA LA SEGUNDA OPCION LLAMADA POR HILOS
  argumentos_t *arg = (argumentos_t *)args;
  int j, k, i;
  unsigned char temp;
  BMP *imagen = arg->imagen;
  int nhilos = arg->nhilos;
  int inicial = arg->ini;
  int finale = arg->fin;  // SE ASIGNA VARIABLES EXTRAIDAS DEL OBJETO CREADO
  for (i = inicial; i <= finale; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      temp = (unsigned char)((imagen->pixel[i][j][2] + imagen->pixel[i][j][1] +
                              imagen->pixel[i][j][0] * 0.11) /
                             3);  // SE BUSCA EL NUMERO QUE SE VA A PONER
      for (k = 0; k < 3; k++) {
        imagen->pixel[i][j][k] = (unsigned char)temp;  // Formula correcta
      }
    }
  }
}
void *tercer(void *args) {  // FUNCION PARA LA TERCERA OPCION LLAMADA POR HILOS
  argumentos_t *arg = (argumentos_t *)args;
  int j, k, i;
  unsigned char temp;
  BMP *imagen = arg->imagen;
  int nhilos = arg->nhilos;
  int inicial = arg->ini;
  int finale = arg->fin;  // SE ASIGNA VARIABLES EXTRAIDAS DEL OBJETO CREADO
  for (i = inicial; i <= finale; i++) {
    for (j = 0; j < imagen->ancho; j++) {
      for (k = 0; k < 3; k++) {
        temp = (unsigned char)abs(imagen->pixel[i][j][k] -
                                  255);  // SE BUSCA EL NUMERO QUE SE VA A PONER
        imagen->pixel[i][j][k] = (unsigned char)temp;  // Formula correcta
      }
    }
  }
}

int main(int argc, char **argv) {
  char imagenIn[50], imagenOut[50];
  int opcion, nhilos;
  BMP img;          // ASIGNAMOS VARIABLES INICIALES
  if (argc != 5) {  // COMPROBAMOS QUE SE META LA CANTIDAD CORRECTA DE DATOS
    fprintf(stderr,
            "Uso: %s <imagen de entrada> <imagen de salida> <opción de "
            "procesamiento> <número de hilos>\n",
            argv[0]);
    exit(1);
  }
  for (int i = 0; argv[3][i] != '\0';
       i++) {  // verifica que el valor ingresado sea un numero
    if (!isdigit(argv[3][i])) {
      printf("La opcion debe ser un número\n");
      return 1;
    }
  }
  for (int i = 0; argv[4][i] != '\0';
       i++) {  // verifica que el valor ingresado sea un numero
    if (!isdigit(argv[4][i])) {
      printf("El numero de hilos debe ser un número\n");
      return 1;
    }
  }
  strcpy(imagenIn, argv[1]);
  strcpy(imagenOut, argv[2]);  // PASAMOS AMBOS TEXTOS A UN VECTOR DE CHAR
  if (atoi(argv[3]) < 1 ||
      atoi(argv[3]) > 3) {  // REVISAMOS QUE LA OPCION ESTE EN EL RANGO DE 1-3
    printf("ingrese un valor entre 1-3");
    exit(1);
  }
  if (atoi(argv[4]) < 1) {  // REVISAMOS QUE MINIMO SE SELECCIONE 1 HILO
    printf("ingrese un numero postivo");
    exit(1);
  }
  opcion = atoi(argv[3]);
  nhilos = atoi(argv[4]);  // SACAMOS EL VALOR DE UN CHAR A UN INT
  pthread_t threads[nhilos];
  argumentos_t
      args[nhilos];  // INICIALISAMOS VARIABLES QUE USAREMOS PARA LOS HILOS
  abrir_imagen(
      &img, imagenIn);  // LLAMAMOS LA FUNCION DE LEER EL ARCHIVO QUE ELEGIMOS
  switch (opcion) {
    case 1:
      printf("Opción 1 seleccionada\n");
      for (int i = 0; i < nhilos; i++) {
        args[i].imagen = &img;
        args[i].nhilos = nhilos;
        args[i].ini =
            i *
            (img.alto /
             nhilos);  // ASIGNAMOS LOS PRIMERO VALORES QUE PASAREMOS AL HILO
        if (i != nhilos - 1) {
          args[i].fin = i * (img.alto / nhilos) + (img.alto / nhilos) -
                        1;  // ASIGNAMOS EL VALOR HASTA EL CUAL LEERA DE LA
                            // ALTUR DE LA IMAGEN
        } else {
          args[i].fin =
              img.alto -
              1;  // SI ESTAMOS EN EL ULTIMO HILO PEGARA EL RSTO DE DATOS
        }
        pthread_create(&threads[i], NULL, primer,
                       (void *)&args[i]);  // CREAMOS EL HILO MANDADNO UN VECTOR
                                           // DEL OBJETO SELECCIONADO
      }

      for (int i = 0; i < nhilos; i++) {
        pthread_join(threads[i], NULL);  // INICIAMOS EL HILO
      }
      break;
    case 2:
      printf("Opción 2 seleccionada\n");
      for (int i = 0; i < nhilos; i++) {
        args[i].imagen = &img;
        args[i].nhilos = nhilos;
        args[i].ini =
            i *
            (img.alto /
             nhilos);  // ASIGNAMOS LOS PRIMERO VALORES QUE PASAREMOS AL HILO
        if (i != nhilos - 1) {
          args[i].fin = i * (img.alto / nhilos) + (img.alto / nhilos) -
                        1;  // ASIGNAMOS EL VALOR HASTA EL CUAL LEERA DE LA
                            // ALTUR DE LA IMAGEN
        } else {
          args[i].fin =
              img.alto -
              1;  // SI ESTAMOS EN EL ULTIMO HILO PEGARA EL RSTO DE DATOS
        }
        pthread_create(&threads[i], NULL, segundo,
                       (void *)&args[i]);  // CREAMOS EL HILO MANDADNO UN VECTOR
                                           // DEL OBJETO SELECCIONADO
      }

      for (int i = 0; i < nhilos; i++) {
        pthread_join(threads[i], NULL);  // INICIAMOS EL HILO
      }
      break;
    case 3:
      printf("Opción 3 seleccionada\n");
      for (int i = 0; i < nhilos; i++) {
        args[i].imagen = &img;
        args[i].nhilos = nhilos;
        args[i].ini =
            i *
            (img.alto /
             nhilos);  // ASIGNAMOS LOS PRIMERO VALORES QUE PASAREMOS AL HILO
        if (i != nhilos - 1) {
          args[i].fin = i * (img.alto / nhilos) + (img.alto / nhilos) -
                        1;  // ASIGNAMOS EL VALOR HASTA EL CUAL LEERA DE LA
                            // ALTUR DE LA IMAGEN
        } else {
          args[i].fin =
              img.alto -
              1;  // SI ESTAMOS EN EL ULTIMO HILO PEGARA EL RSTO DE DATOS
        }
        pthread_create(&threads[i], NULL, tercer,
                       (void *)&args[i]);  // CREAMOS EL HILO MANDADNO UN VECTOR
                                           // DEL OBJETO SELECCIONADO
      }

      for (int i = 0; i < nhilos; i++) {
        pthread_join(threads[i], NULL);  // INICIAMOS EL HILO
      }
      break;
    default:
      printf("Opción no valida\n");
      exit(1);
  }
  crear_imagen(
      &img,
      imagenOut);      // CON LA IMAGEN YA EDITADA LA GUARDAMOS EN OTRO ARCHIVO
  pthread_exit(NULL);  // CERRAMOS LOS HILOS
  printf("imagen convertida\n");
  printf("\n");
  exit(1);
}
