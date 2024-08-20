#include "cntloop.h"
#include "global.h"

static int searchs_graph(Tgraph* graph, int u, int v){
    int ans = 0;
    //printf("%d %d\n",u,v);
   // printf("%d-%d\n %d-%d\n",u,graph->nbAdj[u], v, graph->nbAdj[v]);

    if(graph ->nbAdj[u] > graph->nbAdj[v]){
        int t = u;
        u = v;
        v = t;
   }

    for(int i = 0;i < graph->nbAdj[u];i++)
    {
        if(graph->adj[u][i] == v)
        {
            ans++;
            break;
        }
    }
    if(ans == 1) return 3;
    return 0;
    for(int i = 0;i < graph->nbAdj[v];i++)
    {
        if(graph->adj[v][i] == u)
        {
            ans += 2;
            break;
        }
    }
 
    //printf("%d %d %d\n",u,v,ans);
    return ans;
}


static void removeVertex(int v, Tgraph* graph){
    for (int i=0; i<graph->nbAdj[v]; i++){
        int sv = graph->adj[v][i];
        int j=0;
        while ((j<graph->nbAdj[sv]) && (graph->adj[sv][j] != v)) j++;
        if (j<graph->nbAdj[sv]){
            graph->adj[sv][j] = graph->adj[sv][--graph->nbAdj[sv]];
            if (graph->edgeDirection[sv][v] == 1) graph->nbSucc[sv]--;
            else if (graph->edgeDirection[sv][v] == 2) graph->nbPred[sv]--;
            graph->edgeDirection[sv][v] = 0;
            graph->edgeDirection[v][sv] = 0;
        }
    }
    graph->nbAdj[v] = 0;
    graph->nbSucc[v] = 0;
    graph->nbPred[v] = 0;
}

static void printGraph_large(Tgraph* graph){
    int i, j, k;
    if (graph->isDirected)
        printf("Directed ");
    else
        printf("Non directed ");
    printf("graph with %d vertices\n",graph->nbVertices);
    for (i=0; i<graph->nbVertices; i++){
        printf("Vertex %d has %d adjacent vertices: ",
               i,graph->nbAdj[i]);
        for (j=0; j<graph->nbAdj[i]; j++){
            k = graph->adj[i][j];
            if (graph->edgeDirection[i][k] == 1)
                printf(" %d(succ)",k);
            else if (graph->edgeDirection[i][k] == 2)
                printf(" %d(pred)",k);
            else if (graph->edgeDirection[i][k] == 3)
                printf(" %d(succ and pred)",k);
            else
                printf("error !");
            
        }
        printf("\n");
        
    }
}

void sorts(Tgraph *graph, int l, int r, int base){
    if(l >= r) return;
    int i = l - 1, j = r + 1, x = graph->adj[base][(l + r) >> 1];
    while(i < j){
        do i++; while(graph->adj[base][i] < x);
        do j--; while(graph->adj[base][j] > x);
        if(i < j){
            int t = graph->adj[base][i];
            graph->adj[base][i] = graph->adj[base][j];
            graph->adj[base][j] = t;
        }
    }
    sorts(graph, l, j, base);
    sorts(graph, j + 1, r, base);
}

static void qs(Tgraph *graph){
    for(int i = 0;i < graph->nbVertices;i++){
        sorts(graph, 0, graph->nbAdj[i] - 1, i);
    }
}




Tgraph* createGraph_large(char* fileName, bool isPatternGraph, int* nbIsolated){
	// reads data in fileName and create the corresponding graph 
    // if isPatternGraph = true, then remove isolated vertices and set nbIsolatedVertices to the number of isolated vertices
	
	FILE* fd;
	int i, j, k;
	Tgraph* graph = (Tgraph*)malloc(sizeof(Tgraph));
	
	if ((fd=fopen(fileName, "r"))==NULL){
		printf("ERROR: Cannot open ascii input file %s\n", fileName); 
		exit(1);
	}
	if (fscanf(fd,"%d",&(graph->nbVertices)) != 1){
		printf("ERROR while reading input file %s\n", fileName); 
		exit(1);
	}

    graph->vertexLabel = (int*)calloc(graph->nbVertices,sizeof(int));
    //graph->edgeLabel = (int**)calloc(graph->nbVertices,sizeof(int*));
    graph->isLoop = (bool*)calloc(graph->nbVertices,sizeof(bool));
    graph->nbAdj = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->nbPred = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->nbSucc = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->edgeDirection = (char**)malloc(graph->nbVertices*sizeof(char*));
	graph->adj = (int**)malloc(graph->nbVertices*sizeof(int*));
    graph->maxDegree = 0;
	for (i=0; i<graph->nbVertices; i++){
        graph->isLoop[i] = false;
		//graph->adj[i] = (int*)malloc(graph->nbVertices*sizeof(int));
		if(isPatternGraph) graph->edgeDirection[i] = (char*)calloc(graph->nbVertices,sizeof(char));
		//graph->edgeLabel[i] = (int*)calloc(graph->nbVertices,sizeof(int));
	}
	int **tmpe = (int**)malloc(graph->nbVertices*sizeof(int*));
	for (i=0; i<graph->nbVertices; i++){
		// read degree of vertex i
		if ((fscanf(fd,"%d",&(graph->nbSucc[i])) != 1) || (graph->nbSucc[i] < 0) || (graph->nbSucc[i]>=graph->nbVertices) || (feof(fd))) {
			printf("ERROR while reading input file %s: Vertex %d has an illegal number of successors (%d should be between 0 and %d)\n", 
				   fileName, i, graph->nbSucc[i], graph->nbVertices); 
			exit(1);
		}
		graph->adj[i] = (int*)malloc(graph->nbSucc[i]*sizeof(int));
		tmpe[i] = (int*)malloc(graph->nbSucc[i]*sizeof(int));
        if (graph->nbSucc[i] > graph->maxDegree)
            graph->maxDegree = graph->nbSucc[i];
        for (j=graph->nbSucc[i]; j>0; j--){
			// read jth successor of i
			if ((fscanf(fd,"%d",&k) != 1) || (k<0) || (k>=graph->nbVertices) || (feof(fd))){
				printf("ERROR while reading input file %s: Successor %d of vertex %d has an illegal value %d (should be between 0 and %d)\n", 
					   fileName, j, i, k, graph->nbVertices); 
				exit(1);
			}
			tmpe[i][j - 1] = k;
            
		}
	}
	fclose(fd);
	
	

	for (i=0; i<graph->nbVertices; i++){
		// read degree of vertex i
        for (j=graph->nbSucc[i]; j>0; j--){
			// read jth successor of i
		   k = tmpe[i][j - 1];
           if (i == k){ // The edge is a loop
                graph->isLoop[i] = true;
            }
            else{
	
	if(isPatternGraph){
                    graph->edgeDirection[i][k] = 3;
                }
                graph->adj[i][graph->nbAdj[i]++] = k;
                graph->nbSucc[i]--;
                continue;
                if ((isPatternGraph && graph->edgeDirection[i][k] == 1) || (!isPatternGraph && searchs_graph(graph,i,k) == 1)){
                    printf("ERROR while reading input file %s (the successors of node %d should be all different)\n",fileName, i);
                    exit(1);
                }
                if ((isPatternGraph && graph->edgeDirection[i][k] == 2) || (!isPatternGraph && searchs_graph(graph,i,k) == 2)){
                    // i is a successor of k and k is a successor of i
                    if(isPatternGraph){
                        graph->edgeDirection[k][i] = 3;
                        graph->edgeDirection[i][k] = 3;
                    }
	  // printf("%d %d\n",i,k);
                    graph->adj[i][graph->nbAdj[i]++] = k;
                    graph->nbPred[i]--;
                    graph->nbSucc[i]--;
                    graph->nbSucc[k]--;
                }
                else{
                    graph->nbPred[k]++;
                    graph->adj[i][graph->nbAdj[i]++] = k;
 	   //printf("%d %d\n",i,k);
                    //graph->adj[k][graph->nbAdj[k]++] = i;
                    if(isPatternGraph){
                        graph->edgeDirection[i][k] = 1;
                        graph->edgeDirection[k][i] = 2;
                    }
                    
                }
            }
		}
	}

	//fclose(fd);
	//qs(graph);
 /*
                for(int i = 0;i < graph->nbVertices;i++){
		for(int j = 0;j < graph->nbAdj[i];j++){
			printf("%d ",graph->adj[i][j]); 
			
		}
                               printf("\n");} 
*/
    if (isPatternGraph){//remove isolated vertices
        int idVertex[graph->nbVertices];
        *nbIsolated = 0;
        for (i=0; i<graph->nbVertices; i++){
            if (graph->nbAdj[i] == 0){
                (*nbIsolated)++;
            }
            idVertex[i] = i-*nbIsolated;
        }
        if (*nbIsolated > 0){
            for (i=0; i<graph->nbVertices; i++){
                int ni = idVertex[i];
                if (graph->nbAdj[i]>0){
                if (ni == i){
                    for (int j=0; j<graph->nbVertices; j++){
                        int nj = idVertex[j];
                        if (graph->nbAdj[j]>0)
                            graph->edgeDirection[i][nj] = graph->edgeDirection[i][j];
                    }
                    for (int j=0; j<graph->nbAdj[i]; j++){
                        int si = graph->adj[i][j];
                        int nsi = idVertex[si];
                        graph->adj[i][j] = nsi;
                    }
                }
                else {
                    graph->isLoop[ni] = graph->isLoop[i];
                    graph->nbAdj[ni] = graph->nbAdj[i];
                    graph->nbPred[ni] = graph->nbPred[i];
                    graph->nbSucc[ni] = graph->nbSucc[i];
                    memset(graph->edgeDirection[ni],0,graph->nbVertices*sizeof(char));
                    for (int j=0; j<graph->nbAdj[i]; j++){
                        int si = graph->adj[i][j];
                        int nsi = idVertex[si];
                        graph->adj[ni][j] = nsi;
                        graph->edgeDirection[ni][nsi] = graph->edgeDirection[i][si];
                    }
                }
                }
            }
        }
        graph->nbVertices -= *nbIsolated;
    }
    
    // computing vertex and edge labels

    for (i=0; i<graph->nbVertices; i++){
        if(!isPatternGraph && (graph->nbAdj[i] > 10 || maxloop < 8)) {
            graph->vertexLabel[i] = 1e9;
            continue;
        }
        for (j=0; j<graph->nbAdj[i]; j++){
            int si=graph->adj[i][j]; //
            for (k=0; k<graph->nbAdj[si]; k++){
                int ssi = graph->adj[si][k]; //
                if (ssi != i){
                   // if (graph->edgeDirection[i][ssi] > 0) graph->edgeLabel[i][ssi]++; // i -> si -> ssi
                    if (isPatternGraph && graph->edgeDirection[ssi][i] > 0) {graph->vertexLabel[i]++; cntloop++; if(graph->vertexLabel[i] > maxloop) maxloop = graph->vertexLabel[i]; }
                    if (!isPatternGraph && searchs_graph(graph,ssi,i) > 0) graph->vertexLabel[i]++;
                }
            }
        }
    }

	/*
	if(isPatternGraph){
		printf("%d %d\n",cntloop, maxloop);
		for(int i = 0;i < graph->nbVertices;i++) printf("%d\n",graph->vertexLabel[i]);
		exit(0);
		}
	*/

	graph->isDirected = false;
	return graph;
	for (i=0; i<graph->nbVertices && !graph->isDirected; i++){
		for (j=0; j<graph->nbAdj[i] && !graph->isDirected; j++){
		    if(isPatternGraph) graph->isDirected = (graph->edgeDirection[i][j] == 1 || graph->edgeDirection[i][j] == 2);
            else graph->isDirected = (searchs_graph(graph,i,graph->adj[i][j]) == 1) || ((searchs_graph(graph,i,graph->adj[i][j]) == 2));
		    //if(isPatternGraph) graph->isDirected = (searchp(i,graph->adj[i][j]) == 1) || (searchp(i,graph->adj[i][j]) == 2);
		    //else graph->isDirected = (searcht(i,graph->adj[i][j]) == 1) || (searcht(i,graph->adj[i][j]) == 2);
			//graph->isDirected = (graph->edgeDirection[i][graph->adj[i][j]]==1) || (graph->edgeDirection[i][graph->adj[i][j]]==2);
		}
	}
	return graph;
}

