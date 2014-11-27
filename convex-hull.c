#include "2dch.c"

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

    coord matriz[11][2] = { {0, 0}, 
                          {5, 0}, 
                          {6, 7}, 
                          {6, 9},
                          {5, 8}, 
                          {4, 3},
                          {3, 2},
                          {4, 2},
                          {6, 1},
                          {5, 4},
                          {3, 1}};
    int N = 11;

	print_hull(matriz, ch2d(matriz, read_points()));

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
