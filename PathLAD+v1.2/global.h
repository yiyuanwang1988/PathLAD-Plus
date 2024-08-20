#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <stdbool.h>
#include <stdio.h> 

typedef struct{
	bool isDirected; // false iff for each edge (i,j), there exists an edge (j,i)
    bool* isLoop; // isLoop[i] = true if there is a loop on vertex i 
	int nbVertices; // Number of vertices 
	int* nbAdj;    // nbAdj[i] = number of vertices j such that (i,j) or (j,i) is an edge 
	int* nbPred;   // nbPred[i] = number of vertices j such that (j,i) is an edge and (i,j) is not an edge
	int* nbSucc;   // nbSucc[i] = number of vertices j such that (i,j) is an edge and (j,i) is not an edge
	int** adj;     // forall j in [0..nbAdj[i]-1], adj[i][j] = jth vertex adjacent to i 
	char** edgeDirection;	// if both (i,j) and (j,i) are edges then edgeDirection[i][j] = 3
							// else if (i,j) is an edge then edgeDirection[i][j] = 1
							// else if (j,i) is an edge then edgeDirection[i][j] = 2
							// else (neither (i,j) nor (j,i) is an edge) edgeDirection[i][j] = 0
    int** edgeLabel; 
    int* vertexLabel; 
    int maxDegree; 
} Tgraph;

typedef struct{
	int *nbVal;    // nbVal[u] = number of values in D[u] 
	int *firstVal; // firstVal[u] = pos in val of the first value of D[u] 
	int *val;      // val[firstVal[u]..firstVal[u]+nbVal[u]-1] = values of D[u] 
	int **posInVal; 
	// If v in D[u] then firstVal[u] <= posInVal[u][v] < firstVal[u]+nbVal[u] 
	//                   and val[posInVal[u][v]] = v
	// otherwise posInVal[u][v] >= firstVal[u]+nbVal[u] 
	int **firstMatch; // firstMatch[u][v] = pos in match of the first vertex of the covering matching of G_(u,v)
	int *matching; // matching[firstMatch[u][v]..firstMatch[u][v]+nbAdj[u]-1] = covering matching of G_(u,v)
	int nextOutToFilter; // position in toFilter of the next pattern node whose domain should be filtered (-1 if no domain to filter) 
	int lastInToFilter; // position in toFilter of the last pattern node whose domain should be filtered 
	int *toFilter;  // contain all pattern nodes whose domain should be filtered 
	bool *markedToFilter;    // markedToFilter[u]=true if u is in toFilter; false otherwise 
	int* globalMatchingP; // globalMatchingP[u] = node of Gt matched to u in globalAllDiff(Np)
	int* globalMatchingT; // globalMatchingT[v] = node of Gp matched to v in globalAllDiff(Np) or -1 if v is not matched 
} Tdomain;

#endif