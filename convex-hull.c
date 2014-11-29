#define DEBUG 0
#define DEBUG_RESULTADO 0
//#define N 5 // cantidad de ciudades.

typedef double coord;
//coord points[N][2], *P[N+1]; /* an extra position is used */
coord **points, **P; /* an extra position is used */

#include "2dch.c"
#include <math.h>

/* funcion para medir tiempo */
double time_diff(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;

    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

    diff = (double)y_ms - (double)x_ms;

    return diff/1000000.0;
}

/**
  Se redefine la función read_points para ocuparla en la tarea.

  @parameter char* filename  nombre del archivo donde se encuentran los datos
  @parameter int   num_input cantidad de datos que contiene el arreglo matriz

  @return parámetro de entrada num_input
 */
int read_points(char *filename, int num_input) {
    int id, i;
    double x,y;
    char line[80];

    FILE *fr;
    fr = fopen (filename, "rt");  

    i = 0;

    while(fgets(line, 80, fr) != NULL)
    {	
        if(line[0] > 57 || line[0] < 48)
            continue;

        sscanf (line, "%d %lf %lf", &id, &x, &y);
        points[i][0] = x;
        points[i][1] = y;

        (DEBUG?printf("insertando punto %d -> (%f, %f)\n", i, points[i][0], points[i][1]):1);
        // guardamos las posiciones x en P.
        P[i] = points[i];
        
        i++;
    }

    fclose(fr);

    // se devuelve el número de elementos que debería procesar
    return num_input;
}

/**
  Calcula la distancia entre 2 puntos.

  @parameter coord* p1 arreglo de tamaños 2
  @parameter coord* p2 arreglos de tamaño 2

  @return double distancia euclidiana entre p1 y p2
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

    // swap entre  pos_p <--> num_ch+1
    swap = P[num_ch+1];
    P[num_ch+1] = P[pos_p];
    P[pos_p] = swap;

    swap = P[pos_base];
    P[pos_base] = P[num_ch+1];
    // se desplazan todos los nodos desde pos_base en adelante en 1 posición
    for (i=pos_base+1;i<=num_ch+1;i++) {
        swap2 = P[i];
        P[i] = swap;
        swap = swap2;
    }

    return ++num_ch;
}

/**
	MAIN:

	RECIBE: 
		1. ARCHIVO TSP CON PUNTOS
		2. NUMERO DE PUNTOS EN ARCHIVO
	REALIZA:
		1. CARGA ARREGLO EN MEMORIA RAM
		2. CALCULA CONVEX-HULL
		3. AGREGA PUNTOS A CONVEX-HULL
        4. IMPRESION DE LARGO DE CAMINO
		5. IMPRESION DE TIEMPO UTILIZADO
        
    OBS: El algoritmo asume que hay por lo menos un punto que agregar 
         para poder calcular la distancia del convex-hull
*/
void main(int argc, char** argv) {

	/***********************************/
	/** 1. init variables			  **/
	/***********************************/
    
    // variables para experimentos
    int N, i;
	struct timeval before , after;
	double time_elapsed;
    // se rescata como parametro
	N = atoi(argv[2]);
    
    // arreglos que utiliza convex-hull
    points = malloc(N * sizeof(coord*));
    for (i = 0; i < N; i++) {
        points[i] = malloc(2 * sizeof(coord));
    }

    P = malloc((N+1)*sizeof(coord*));

    // ************************************************************
    int num_ch, j, k, elegido;
    int rows = N;
    int cols = 2;
    double min_dist, min_form, aux, distancia_final=0;
    int calc_dist_ch = 0; // flag
    coord **matriz;

    matriz = malloc(rows * sizeof(coord *));
    for (i = 0; i < rows; i++) {
        matriz[i] = malloc(cols * sizeof(coord));
    }
    
    int **trios;
    trios = malloc((N+1) * sizeof(int *));
    for(i=0;i<N+1;i++) {
        trios[i] = malloc(3 * sizeof(int));
    }

    /**************************************************************************************************/
    /**************************************************************************************************/
    // se comienza a medir el tiempo
	gettimeofday(&before, NULL);

    // carga los datos en los arreglos que ocupa ch2d
    read_points(argv[1], N);
    // se calcula el convex hull y se guarda en P. El valor retornado indica la cantidad de elementos que
    // forman parte del convex-hull y que están al principio de P.
    num_ch = ch2d(P, N);
    
    (DEBUG?print_hull(P, num_ch):1);

    // OBS: los puntos del covex-hull se encuentran en los primeros num_ch puntos de P.
    // Mientras existan puntos que están fuera del convex-hull
    while(num_ch < N) {

        // todos los puntos en P con indice > num_ch no están en el convex hull.
        // Se ocupa el punto P[num_ch] para dejar el primer elemento y así cerrar el ciclo (por eso se define como P[N+1]).
        // por lo tanto los elementos que no están en el vertex-hull están en num_ch+1, ...
        for(i=num_ch+1;i<=N;i++) {
            (DEBUG?printf("No está en convex-hull (%d)-> (%f, %f).\n", i, P[i][0], P[i][1]):1);

            min_dist = 1000000; // ínfinito
            
            // si no se ha calculado la distancia del convex-hull aún se activa el flag
            if (distancia_final == 0)
                calc_dist_ch = 1;

            // encontrar par p1,p2 para punto i tal que minimiza distancia a convex-hull.
            // OBS: no nos preocupamos del caso limite porque P[num_ch] tiene el primer elemento.
            for(j=0;j<num_ch;j++) {
                    // se calcula la distancia de la envoltura convexa
                    if(calc_dist_ch) {
                        distancia_final+=distancia(P[j], P[j+1]); 
                        //printf("sumar %f\n", distancia(P[j], P[j+1]));
                    }

                    aux = distancia(P[i], P[j]) + distancia(P[i], P[j+1]) - distancia(P[j], P[j+1]);
                    //printf("%f+%f-%f=%f\n", distancia(P[i], P[j]), distancia(P[i], P[j+1]), distancia(P[j], P[j+1]), aux);
                    if (aux < min_dist) {
                        min_dist = aux;
                        // se actualizan los puntos
                        trios[i][0] = j; // p1
                        trios[i][1] = i; // p
                        trios[i][2] = j+1; // p2
                    }
            }

            // se calcula una sola vez la envoltura convexa.
            calc_dist_ch = 0;

            (DEBUG?printf("    (%f, %f) va entre (%f,%f) y (%f,%f).\n", P[i][0], P[i][1], 
                                                                        P[trios[i][0]][0], P[trios[i][0]][1], 
                                                                        P[trios[i][2]][0], P[trios[i][2]][1]):1);
        }
 
        (DEBUG?printf("\nAhora elegimos el P* que maximiza la 2° fórmula\n\n"):1); 

        // encontrar p* tal que es el mínimo en la nueva fórmula
        min_form = 1000000;
        elegido = num_ch+1; // por default el primer p* mínimiza la fórmula.
        for(i=num_ch+1;i<=N;i++) {

            aux = (distancia(P[trios[i][1]], P[trios[i][0]]) + 
                    distancia(P[trios[i][1]], P[trios[i][2]]))/
                    distancia(P[trios[i][0]], P[trios[i][2]]);

            if (aux < min_form) {
                // guardar indices
                min_form = aux;
                elegido = i;
            }
        }

        // Se calcula la distancia final
        distancia_final += distancia(P[trios[elegido][0]], P[trios[elegido][1]]) + 
                           distancia(P[trios[elegido][1]], P[trios[elegido][2]]) - 
                           distancia(P[trios[elegido][0]], P[trios[elegido][2]]);

        (DEBUG?printf("    Insertar en Convex-Hull ->  (%f, %f)\n", P[elegido][0], P[elegido][1]):1);
        // insertar en convex hull en punto escogido
        num_ch = agregar_a_ch(trios[elegido][1], trios[elegido][2], num_ch);
    }

    // fin de medición de tiempo
	gettimeofday(&after, NULL);
	time_elapsed = time_diff(before, after);

	/***********************************/
	/** 3. Finish program			  **/
	/***********************************/

    (DEBUG_RESULTADO?printf("Distancia total recorrida: %f\n", distancia_final):1);
    (DEBUG_RESULTADO?printf("Tiempo de computo: %f\n", time_elapsed):1);
    //(DEBUG_RESULTADO?print_hull(P, num_ch):1);
    //(DEBUG_RESULTADO?printf("\n========================================\n"):1);

    exit(0);
}

