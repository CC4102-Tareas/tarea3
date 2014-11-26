


/**
    Grafo G = (V,E) basada en una matriz de adyacencia 
    con pesos A.

      1 2 3 4 5 ...
     --------------
    1|2 4 7 8 3 ...
    2|. .
    3|.   .
    4|.     .
    5|
     --------------

    Cada celda C_ij representa el costo del camino que 
    hay entre la ciudad i y la ciudad j.
*/





/**
    Construir MST con el Algoritmo de Kruskal.

    Previamente generar otra Matriz de adyacencia con pesos B.

    Alg de Kruskal->
        - Ordenar aristas de menor a mayor costo.
        - iterar sobre cada arista de forma ascendente.
        - Para cada arista i insertar
            si se forma un ciclo -> descartar arista i
            sino seguir.

    Fin, tenemos MST.
*/


/**
    Elegimos una raíz r cualquiera dentro de los nodos (al azar).
    Procedimiento:

        - Escoger raíz r.
        - A partir de r recorrer B y crear el ABB C.

*/


/**
    recorrer árbol C en pre-orden
*/

