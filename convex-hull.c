#include "2dch.c"

/**
  Se redefine la función read_points para ocuparla en la tarea.
 */
int read_points(coord **matriz, int num_input) {
    char buf[100];
    int n = 0;

    //points = matriz;

    while (n < num_input) {
        // se traspasan los datos al nuevo arreglo.
        // OBS: se podría hacer algo más eficiente como que points sea un 
        //      puntero que apunte al nuevo arreglo pero bueno...se acabo el semestre! :P
        points[n][0] = 5000;
        points[n][1] = 5000;

        printf("insertando punto %d -> (%d, %d)\n", n, points[n][0], points[n][1]);
        // guardamos las posiciones x en P.
        P[n] = points[n];
        // revisa si se rompe el contrato.
        assert(++n <= N);
    }

    // se devuelve el número de elementos que debería procesar
    return num_input;
}

/**
  Convex-Hull -> conjunto de puntos de P más pequeño que contiene todos 
  los puntos.
  Sea C subconjunto de P tal que es un convex-hull.

Procedimiento:

1) Calcular convex-hull del conjunto de entrada. Se obtiene un conjunto C.
 */

void main(int argc, char** argv) {

    // 1° arg: número de ciudades.
    // 2° arg: matriz con las ubicaciones de las ciudades.
    //int N = atoi(argv[1]);
    //int *matriz = argv[2];
    
    int i;
    int rows = 11;
    int cols = 2;
    coord **matriz;

    matriz = malloc(rows * sizeof(coord *));
    for (i = 0; i < rows; i++) {
        matriz[i] = malloc(cols * sizeof(coord));
    }
    
    int j;
// Some testing
for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++)
              matriz[i][j] = 0; // or whatever you want
}

for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++)
              printf("%d ", matriz[i][j]);
}
exit(0);

    matriz[0][0] = 0;matriz[0][1] = 0;     
    matriz[1][0] = 5;matriz[1][1] = 0;
    matriz[2][0] = 6;matriz[2][1] = 7;
    matriz[3][0] = 6;matriz[3][1] = 9;
    matriz[4][0] = 5;matriz[4][1] = 8;
    matriz[5][0] = 4;matriz[5][1] = 3;
    matriz[6][0] = 3;matriz[6][1] = 2;
    matriz[7][0] = 4;matriz[7][1] = 2;
    matriz[8][0] = 6;matriz[8][1] = 1;
    matriz[9][0] = 5;matriz[9][1] = 4;
    matriz[10][0] = 3;matriz[10][1] = 1;

    int n = 0;
    while (n<11) {
        printf("insertando punto %d -> (%d, %d)\n", n, matriz[n][0], matriz[n][1]);
        n++;
    }

    printf("Bien hasta aquí\n");
    exit(0);

    print_hull(matriz, ch2d(P, read_points(matriz, rows)));

    exit(0);
}

/**
  2) Se recorren todos los puntos fuera de C.
  2.1) En cada paso donde se encuentre un punto p que no está en C,
  buscar arco (p1, p2) en C, que minimiza la distancia siguiente

  dist(p,p1) + dist(p,p2) - dist(p1,p2)   O(|C|^2)

  2.2) El paso anterior puede entregar más de un par, luego
  se escoge el punto p* del conjunto encontrado en el punto anterior el
  que minimiza

  dist(p*, p1*) + dist(p*, p2*)
  ----------------------------
  dist(p1*, p2*)

  2.3) Extender C incluyendo p* entre p1* y p2*.

  2.4) Si no quedan elementos fuera de C, retornar C.
 */
