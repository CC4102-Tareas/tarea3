#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef unsigned long marker;
marker one = 1;

double ** comb_matrix;
double ** random_points;
int ** index_min;

/* funcion para medir tiempo */
double time_diff(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;
     
    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
     
    diff = (double)y_ms - (double)x_ms;
     
    return diff/1000000.0;
}

double distance(int p1, int p2){
	return sqrt(pow(random_points[p1][0]-random_points[p2][0],2.0)+pow(random_points[p1][1]-random_points[p2][1],2.0));
}

/* funcion para llenar una fila de la tabla de acuerdo a la recursion propuesta*/
void fillTable (int row){
	int i,j, ind_min;
	int n_ones = 0;
	double min_dist;
	double aux_distance;
	int row_with_removed_element;

	for (i = 0; i < 20; i++)
		if((one << i) & row)	n_ones++;
	
	if (n_ones == 1){

		for (i = 0; i < 20; i++){
			if((one << i) & row)
			{
				comb_matrix[row][i] = distance(0,i+1);
			}
		}

	}
	else{

		for (i = 0; i < 20; i++)
		{
			if((one << i) & row)	
			{	
				row_with_removed_element = row - (one << i);
				min_dist = -1;
				ind_min = 0;
				for (j = 0; j < 20; j++)
				{
					if((one << j) & row_with_removed_element)
					{
						aux_distance = comb_matrix[row_with_removed_element][j] + distance(i+1,j+1);
						if (min_dist < 0 || aux_distance < min_dist )
						{
							min_dist = aux_distance;
							ind_min = j;
						}
					}
				}

				comb_matrix[row][i] = min_dist;
				index_min[row][i] = ind_min;
			}
		}
	}

	return;
}

/* funcion para llenar la tabla ordenadamente */
void comb(int pool, int need, marker chosen, int at)
{
	if (pool < need + at) return; /* not enough bits left */
 
	int index = 0; int i;
	if (!need) {
		for (at = 0; at < pool; at++)
			if (chosen & (one << at)) index = index + (1 << at);
			fillTable(index);
		return;
	}

	comb(pool, need - 1, chosen | (one << at), at + 1);
	comb(pool, need, chosen, at + 1);
}

/* funcion para leer archivos tsp*/
void readtsp(char * filename, double ** points)
{
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
		i++;
   }

   fclose(fr);
}


float float_aleatorio (float a){

	return ((float)rand()/(float)(RAND_MAX)) * a;

}

int int_aleatorio (float a){

	return (int) (((float)rand()/(float)(RAND_MAX)) * a);

}

/* 
	MAIN:

	RECIBE: 
		1. ARCHIVO TSP CON PUNTOS
		2. NUMERO DE PUNTOS EN ARCHIVO
		3. NUMERO DE ELEMENTOS A ELEGIR (n = {5, ..., 20})
	REALIZA:
		1. ELECCION ALEATORIZADA DE n ELEMENTOS
		2. BÚSQUEDA CAMINO OPTIMO
		3. IMPRESION DE LARGO DE CAMINO
		4. IMPRESION DE TIEMPO UTILIZADO
*/

int main(int argc, char ** argv){

	/***********************************/
	/** 1. init variables			    **/
	/***********************************/

	srand (time(NULL));
	int n_points, i, n, j, rnd, rnd_size, repeated;
	double ** points;
	struct timeval before , after;
	double time_elapsed;

	n_points = atoi(argv[2]);
	n = atoi(argv[3]);

	int random_index[n];
	rnd_size = 0;
	
	points = malloc(n_points * sizeof(double * ));
	random_points = malloc(n * sizeof(double * ));
	
	for (i = 0; i < n_points; i++)
		points[i] = malloc(2 * sizeof(double) );

	readtsp(argv[1], points);

	for (i = 0; i < n; i++){
			random_points[i] = malloc(2 * sizeof(double) );
	}

	/***********************************/
	/** 2. select n points randomly   **/
	/***********************************/
	for (i = 0; i < n; i++){

		while(1){
			rnd = int_aleatorio((float)n_points);
			repeated = 0;
			for (j = 0; j < rnd_size; j++){
				if (rnd == random_index[j]) 
					repeated = 1;
			}
			if (repeated){
				continue;
			}
			else
				break;
		}
		random_index[i] = rnd;
		random_points[i][0] = points[rnd][0];
		random_points[i][1] = points[rnd][1];
		rnd_size++;
	}

	for (i = 0; i < n_points; i++){
		free(points[i]);
		}
	free(points);

	/***********************************/
	/** 3. Calculate optimal circuit	 **/
	/***********************************/

	// number of combinations
	int n_comb = 1 << (n-1);

	// matrix with combinations
	comb_matrix = malloc(n_comb * sizeof(double * ));

	// matrix with last point visited
	index_min = malloc(n_comb * sizeof(int * ));	

	for (i = 0; i < n_comb; i++)
	{
		comb_matrix[i] = malloc((n-1) * sizeof(double) );
		index_min[i] = malloc((n-1) * sizeof(int) );
	}

	double min_dist = -1.0;
	double aux_dist;

	int bits = n-1;
	int set = n_comb-1;

	gettimeofday(&before, NULL);
	/* paso 1: llenar tabla */
	for (i = 1; i <= bits; i++)
		comb(bits, i, 0, 0);

	/* paso 2.1: encontrar, dentro del conjunto de todos los puntos,
		el punto que genera el minimo camino hamiltoniano.	*/
	for (i = 0; i < bits; i++)
	{
		aux_dist = comb_matrix[n_comb-1][i] + distance(0,i+1);
		if(min_dist < 0.0 || aux_dist < min_dist)
		{	
			min_dist = aux_dist;
			random_index[0] = i;
		}
	}

	/* paso 2.2: reconstruir el camino hacia p1 mirando qué punto dio
		origen al conjunto actual. */
	for (i = 1; i < bits; i++)
	{
		random_index[i] = index_min[set][random_index[i-1]];
		set = set - (1<<random_index[i-1]);
	}

	random_index[bits] = -1;
	gettimeofday(&after, NULL);	
	time_elapsed = time_diff(before, after);

	printf("Puntos recorridos en orden:\n");
	printf("[%f, %f]\n", random_points[0][0], random_points[0][1]);
	for (i = 0; i <= bits; i++)
	{	
		printf("[%f, %f]\n", random_points[random_index[i]+1][0], random_points[random_index[i]+1][1]);
	}

	printf("Distancia total recorrida: %f\n", min_dist);
	printf("Tiempo de computo: %f\n", time_elapsed);

	/***********************************/
	/** 4. Finish program			    **/
	/***********************************/

	
	for (i = 0; i < n; i++){
		free(random_points[i]);
	}
	free(random_points);
	
	for (i = 0; i < n_comb; i++){
		free(comb_matrix[i]);
		free(index_min[i]);
	}
	free(comb_matrix);
	free(index_min);
	return;
}
