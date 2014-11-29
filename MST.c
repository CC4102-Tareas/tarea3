#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
 
#define DEBUG 0

double ** points; 		// puntos leidos desde archivo
int * position_in_heap; // position de los puntos en el 
double min_key; 			// llave minima en el heap
double total_dist; 		// distancia total recorrida
int last_point;			// ultimo punto visitado

/* cola de prioridad */
typedef struct
{
	int heapsize;
	int heaplength;
	int * queue;
	double * key;

} heap;

/* arbol n-ario */
typedef struct t {
	int id;
	struct t *children;
	struct t *siblings;
} n_ary_tree;

/* funcion distancia entre 2 puntos del arreglo */
double distance(int p1, int p2){
	return sqrt(pow(points[p1][0]-points[p2][0],2.0)+pow(points[p1][1]-points[p2][1],2.0));
}

/** funciones del arbol m-ario **/

/* agregar "hermano" */
void add_sibling(n_ary_tree * node, n_ary_tree * new_node)
{
	if (node->siblings == NULL)
		node->siblings = new_node;
	else
		add_sibling(node->siblings, new_node);
}

/* agregar hijo */
void add_child(n_ary_tree * partner, n_ary_tree * child)
{
	if (partner->children==NULL )
		partner->children = child;
	else
	{
		add_sibling(partner->children, child);
	}
}		

/* recorrer arbol en preorden */
void preorder(n_ary_tree *root)
{
	if(root != NULL)
   {
		if (last_point != -1)
		{
			total_dist = total_dist + distance(root->id, last_point);
		}
		last_point = root->id;
		preorder(root->children);
		preorder(root->siblings);
	}
	return;
}

/** funciones del heap **/

/* funcion para rescatar nodos padre e hijos */
int get_parent(i)
{
	return (i-1)/2;
}

int get_left(i)
{
	return 2*i+1;
}

int get_right(i)
{
	return 2*i+2;
}

/* heapify para heap segun llave minima */
void min_heapify(heap * H, int i)
{
	int l = get_left(i);
	int r = get_right(i);
	int smallest;

	if (l < H->heapsize && H->key[l] < H->key[i])
		smallest = l;
	else
		smallest = i;

	if (r < H->heapsize && H->key[r] < H->key[smallest])
		smallest = r;

	if (smallest != i)
	{
		double aux = H->key[i];
		int aux_q = H->queue[i];

		H->key[i] = H->key[smallest];
		H->queue[i] = H->queue[smallest];
		position_in_heap[H->queue[i]] = i;

		H->key[smallest] = aux;
		H->queue[smallest] = aux_q;
		position_in_heap[H->queue[smallest]] = smallest;

		min_heapify(H, smallest);
	}

}

/* "heapificar" arreglo preexistente en el heap H */
void build_min_heap (heap * H)
{
	int i;
	H->heapsize = H->heaplength;
	for ( i = H->heaplength/2; i >= 0; i--)
	{
		min_heapify(H,i);
	}
}

/* extraer minimo de la cola de prioridad */
int extract_min(heap * H)
{
	if (H->heapsize < 1)
		printf("extract_min: error\n");

	min_key = H->key[0];

	int min_q = H->queue[0];

	H->key[0] = H->key[H->heapsize-1];
	H->queue[0] = H->queue[H->heapsize-1];
	position_in_heap[H->queue[H->heapsize-1]] = 0;
	H->heapsize = H->heapsize - 1;
	min_heapify(H, 0);
	return min_q;
}

/* actualizar heap cuando se actualiza una llave en la posicion i */
void decrease_key(heap * H, int i, double new_key)
{
	if (new_key > H->key[i])
		printf("decrease key: error\n");

	H->key[i] = new_key;

	while (i > 0 && H->key[get_parent(i)] > H->key[i])
	{
		double aux = H->key[i];
		int aux_q = H->queue[i];

		H->key[i] = H->key[get_parent(i)];
		H->queue[i] = H->queue[get_parent(i)];
		position_in_heap[H->queue[i]] = i;

		H->key[get_parent(i)] = aux;
		H->queue[get_parent(i)] = aux_q;
		position_in_heap[H->queue[get_parent(i)]] = get_parent(i);
	
		i = get_parent(i);
	
	}
}

/* funcion para calcular minimum spanning tree */
void mst_prim (int n_points)
{
	int i, j, node;
	heap * H;
	double * key;
	int * parent;
	char * in_queue;

	/* pedir memoria para estructuras auxiliares */
	H = malloc(sizeof(heap));
	H->heapsize = n_points;
	H->heaplength = n_points;
	H->key = malloc(n_points * sizeof(double));
	H->queue = malloc(n_points * sizeof(int));
	parent = malloc(n_points * sizeof(int));
	in_queue = malloc(n_points * sizeof(char));
	n_ary_tree * nodes = malloc(n_points * sizeof(n_ary_tree));

	/* inicializar */
	for (i = 0; i < n_points; i++)

	{
		H->key[i] = DBL_MAX;
		parent[i] = -1;
		H->queue[i] = i;
		position_in_heap[i] = i;
		in_queue[i] = 1;
		(&nodes[i])->id = i;
		(&nodes[i])->siblings = NULL;
		(&nodes[i])->children = NULL;
	}
	H->key[0] = 0.0;

	/* generar arbol con algoritmo de prim */
	for (i = 0; i < n_points; i++)
	{
		node = extract_min(H);
		in_queue[node] = 0;
		int p = parent[node];
		if (p > -1)
			add_child(&nodes[p], &nodes[node]);

		for (j = 0; j < n_points; j++)
		{
			if (j != node)
			{
				int q = position_in_heap[j];
				if ( in_queue[j] && distance(node,j) < H->key[q])
				{	
					parent[j] = node;
					decrease_key(H, q, distance(node,j));
				}
			}
		}
	}

	free(H->queue);
	free(H->key);
	free(parent);
	free(in_queue);
	free(H);	
	preorder(&nodes[0]);
	total_dist = total_dist+distance(last_point,0);
	free(nodes);
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

/* funcion para medir tiempo */
double time_diff(struct timeval x , struct timeval y)
{
    double x_ms , y_ms , diff;
     
    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
     
    diff = (double)y_ms - (double)x_ms;
     
    return diff/1000000.0;
}

/* 
	MAIN:

	RECIBE: 
		1. ARCHIVO TSP CON PUNTOS
		2. NUMERO DE PUNTOS EN ARCHIVO
	REALIZA:
		1. CONSTRUCCION DE ARBOL DE COBERTURA MINIMO (MST)
		2. CONSTRUCCION DE CAMINO HAMILTONIANO A PARTIR DE (MST)
		3. IMPRESION DE LARGO DE CAMINO
		4. IMPRESION DE TIEMPO UTILIZADO
*/
int main(int argc, char ** argv){

	/***********************************/
	/** 1. init variables			    **/
	/***********************************/

	int n_points, i;
	struct timeval before , after;
	double time_elapsed;
	n_points = atoi(argv[2]);
	
	total_dist = 0.0;
	last_point = -1;
	points = malloc(n_points * sizeof(double * ));
	position_in_heap = malloc(n_points * sizeof(int));	
	
	for (i = 0; i < n_points; i++)
		points[i] = malloc(2 * sizeof(double) );

	/* llevo puntos desde disco a RAM (arreglo points) */
	readtsp(argv[1], points);	

	/******************************************************/
	/** 2. Calculate optimal circuit	(MST-Approximation) **/
	/******************************************************/

	/* funcion mst_prim calcula mst y construye camino sub-optimo*/

	gettimeofday(&before, NULL);
	mst_prim (n_points);
	gettimeofday(&after, NULL);
	time_elapsed = time_diff(before, after);

	/***********************************/
	/** 3. Finish program			    **/
	/***********************************/

	printf("Distancia total recorrida: %f\n", total_dist);
	printf("Tiempo de computo: %f\n", time_elapsed);

	for (i = 0; i < n_points; i++){
		free(points[i]);
		}
	free(points);

	free(position_in_heap);

	return;
}
