#define DEBUG 1
#define N 11 // cantidad de ciudades.

typedef double coord;
coord points[N][2], *P[N+1]; /* an extra position is used */

#include "2dch.c"
#include <math.h>

/**
  Se redefine la función read_points para ocuparla en la tarea.
 */
int read_points(coord **matriz, int num_input) {
    int n = 0;

    //points = matriz;

    while (n < num_input) {
        // se traspasan los datos al nuevo arreglo.
        // OBS: se podría hacer algo más eficiente como que points sea un 
        //      puntero que apunte al nuevo arreglo pero bueno...se acabo el semestre! :P
        points[n][0] = matriz[n][0];
        points[n][1] = matriz[n][1];

        (DEBUG?printf("insertando punto %d -> (%f, %f)\n", n, points[n][0], points[n][1]):1);
        // guardamos las posiciones x en P.
        P[n] = points[n];

        // revisa si se rompe el contrato.
        assert(++n <= N);
    }

    // se devuelve el número de elementos que debería procesar
    return num_input;
}

/**
  Calcula la distancia entre 2 puntos.
 */
double distancia(coord *p1, coord *p2) {
    coord x1, x2, y1, y2;

    x1 = p1[0];
    y1 = p1[1];
    x2 = p2[0];
    y2 = p2[1];

    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));   
}

/**
  Extiende convex hull agregando un nodo adicional.

  @parameter coord* point  punto para agregar al convex hull
  @parameter int    pos    posición del punto point dentro de P.
  @parameter int    num_ch cantidad de elementos del convex-hull

  @return int nueva cantidad de elementos en el convex-hull
 */
int agregar_a_ch(int pos_p, int pos_base, int num_ch) {

    int i;
    coord *swap, *swap2;

    swap = P[pos_base];
    // se inserta nodo en su posición.
    P[pos_base] = P[pos_p];
    // se desplazan todos los nodos desde pos_base en adelante en 1 posición
    for (i=pos_base+1;i<=num_ch;i+=2) {
        swap2 = P[i];
        P[i] = swap;
        swap = swap2;
    }

    return ++num_ch;
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
    int rows = N;
    int cols = 2;
    coord **matriz;

    matriz = malloc(rows * sizeof(coord *));
    for (i = 0; i < rows; i++) {
        matriz[i] = malloc(cols * sizeof(coord));
    }

    // caso de prueba   
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

    /**************************************************************************************************/
    /**************************************************************************************************/
    // carga los datos en los arreglos que ocupa ch2d
    read_points(matriz, N);
    // se calcula el convex hull y se guarda en P. El valor retornado indica la cantidad de elementos que
    // forman parte del convex-hull y que están al principio de P.
    int num_ch = ch2d(P, N);
    print_hull(P, num_ch);

    // los puntos de la covex-hull se encuentran en los primeros num_ch puntos de P.

    /**
      2) Se recorren todos los puntos fuera de P.
      2.1) En cada paso donde se encuentre un punto p que no está en P,
      buscar arco (p1, p2) en C, que minimiza lo siguiente

      dist(p,p1) + dist(p,p2) - dist(p1,p2)   O(|C|^2)
     */

    // almacena todos los trios, p1 p p2, donde p1 y p2 son los puntos 
    // que minimizan la distancia a p desde el vertex-hull.
    //coord **trios = malloc(N * sizeof(coord*));
    int trios[N][3];
    double min_dist, min_form, aux;
    int j,k, elegido;

    // Mientras existan puntos que están fuera del convex-hull
    while(num_ch <= N) {

        // todos los puntos en P con indice >= num_ch no están en el convex hull.
        for(i=num_ch+1;i<=N;i++) {
            (DEBUG?printf("No está en convex-hull %d -> (%f, %f).\n", i, P[i][0], P[i][1]):1);

            // encontrar par p1,p2 para punto i tal que minimiza distancia a convex-hull.
            min_dist = 1000000; // ínfinito
            // evaluamos todos con todos
            for(j=0;j<num_ch;j+=2) {
                for(k=j+1;k<num_ch;k++) {                
                    aux = distancia(P[i], P[j]) + distancia(P[i], P[k]) - distancia(P[j], P[k]);
                    if (aux < min_dist) {
                        min_dist = aux;
                        // se actualizan los puntos
                        trios[i][0] = j; // p1
                        trios[i][1] = i; // p
                        trios[i][2] = k; // p2
                    }
                }
            }
        }

        // encontrar p* tal que es el mínimo en la nueva fórmula
        min_form = 1000000;
        elegido = -1;
        for(i=num_ch;i<N;i++) {
            aux = (distancia(P[trios[i][1]], P[trios[i][0]]) + distancia(P[trios[i][1]], P[trios[i][2]]))/distancia(P[trios[i][0]], P[trios[i][2]]);
            if (aux < min_form) {
                // guardar indices
                elegido = i;
            }
        }

        printf("    Insertar en Convex-Hull ->  (%f, %f)\n", P[elegido][0], P[elegido][1]);
        // insertar en convex hull en punto escogido
        num_ch = agregar_a_ch(trios[elegido][1], trios[elegido][2], num_ch);
        //printf("\nResultado final ->\n");
        //print_hull(P, num_ch);
    }

    exit(0);
}

/**
  2.2) El paso anterior puede entregar más de un par, luego
  se escoge el punto p* del conjunto encontrado en el punto anterior el
  que minimiza

  dist(p*, p1*) + dist(p*, p2*)
  ----------------------------
  dist(p1*, p2*)

  2.3) Extender C incluyendo p* entre p1* y p2*.

  2.4) Si no quedan elementos fuera de C, retornar C.
 */
