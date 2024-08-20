#pragma GCC optimize(1)
#pragma GCC optimize(2)
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline") 
#pragma GCC optimize("-fgcse")
#pragma GCC optimize("-fgcse-lm")
#pragma GCC optimize("-fipa-sra")
#pragma GCC optimize("-ftree-pre")
#pragma GCC optimize("-ftree-vrp")
#pragma GCC optimize("-fpeephole2")
#pragma GCC optimize("-ffast-math")
#pragma GCC optimize("-fsched-spec")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("-falign-jumps")
#pragma GCC optimize("-falign-loops")
#pragma GCC optimize("-falign-labels")
#pragma GCC optimize("-fdevirtualize")
#pragma GCC optimize("-fcaller-saves")
#pragma GCC optimize("-fcrossjumping")
#pragma GCC optimize("-fthread-jumps")
#pragma GCC optimize("-funroll-loops")
#pragma GCC optimize("-freorder-blocks")
#pragma GCC optimize("-fschedule-insns")
#pragma GCC optimize("inline-functions")
#pragma GCC optimize("-ftree-tail-merge")
#pragma GCC optimize("-fschedule-insns2")
#pragma GCC optimize("-fstrict-aliasing")
#pragma GCC optimize("-falign-functions")
#pragma GCC optimize("-fcse-follow-jumps")
#pragma GCC optimize("-fsched-interblock")
#pragma GCC optimize("-fpartial-inlining")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("-freorder-functions")
#pragma GCC optimize("-findirect-inlining")
#pragma GCC optimize("-fhoist-adjacent-loads")
#pragma GCC optimize("-frerun-cse-after-loop")
#pragma GCC optimize("inline-small-functions")
#pragma GCC optimize("-finline-small-functions")
#pragma GCC optimize("-ftree-switch-conversion")
#pragma GCC optimize("-foptimize-sibling-calls")
#pragma GCC optimize("-fexpensive-optimizations")
#pragma GCC optimize("inline-functions-called-once")
#pragma GCC optimize("-fdelete-null-pointer-checks")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/resource.h>

// define boolean type as char
#define true 1
#define false 0
#define bool char

// Global variables
static int nbNodes = 1;      // number of nodes in the search tree
static int nbFail = 0;       // number of failed nodes in the search tree
static int nbSol = 0;        // number of solutions found
static struct rusage ru;     // reusable structure to get CPU time usage


#include "compatibility.c"
#include "graph_large.c"
#include "domainsPath_large.c"
#include "allDiff_large.c"
#include "lad_large.c"
#include "cntloop.h"

#include "solve_large.h"

int cntloop = 0, maxloop = 0;

int upsol = 100000;

void quicks(int *q, int l, int r, Tgraph* Gt)
{
    if(l>=r) return;
    int i = l - 1, j = r + 1, x = Gt->nbAdj[q[l + r >> 1]];

    while(i < j)
    {
        do i++; while(Gt->nbAdj[q[i]] > x );
        do j--;  while(Gt->nbAdj[q[j]]  < x );
        if(i < j) 
        {
            int t = q[i];
            q[i] = q[j];
            q[j] = t;
        }
    }
    quicks(q,l,j,Gt);
    quicks(q,j+1,r,Gt);
}


int printStats_large(bool timeout){
	// print statistics line and return exit status depending on timeout
//printf("%d\n",cnt);
	getrusage(RUSAGE_SELF, &ru);
	if (timeout)
		printf("CPU time exceeded");
	else
		printf("Run completed");
	printf(": %d solutions; %d fail nodes; %d nodes; %d.%06d seconds\n",
		   nbSol, nbFail, nbNodes,
		   (int) ru.ru_utime.tv_sec, (int) ru.ru_utime.tv_usec);
	return timeout;
}



void exc_large(int* a, int* b)
{
    int t;
    t = *a;
    *a = *b;
    *b = t;
}

void disorganise_large(int a[], int len)
{
    //srand( time(NULL) );
    // Fisher-Yates shuffle
    while(len > 0){
        int tmp = rand() % len;
        exc_large(&a[tmp], &a[len - 1]);
        len--;
    }
    /*
    int rN1 = (rand() % len);
    int rN2 = (rand() % len);
    for (int i = 0; i < len + 2; i++) {
        if (rN1 != rN2) {
            exc(&a[rN1], &a[rN2]);
        }
        rN1 = (rand() % len);
        rN2 = (rand() % len);
    }
    */
}



bool solve_large(int timeLimit, bool firstSol, bool induced, int verbose, Tdomain* D, Tgraph* Gp, Tgraph* Gt){
    // if firstSol then search for the first solution; otherwise search for all solutions
    // if induced then search for induced subgraphs; otherwise search for partial subgraphs
    // return false if CPU time limit exceeded before the search is completed
    // return true otherwise
  
    int u, v, nextVar, i;
    int* nbVal = malloc(sizeof(int) * Gp->nbVertices);
    nbNodes++;
    //int nbVal[Gp->nbVertices];
    //int globalMatching[Gp->nbVertices];
    //int* globalMatching = malloc(sizeof(int) * Gp->nbVertices);
    getrusage(RUSAGE_SELF, &ru);
    if (ru.ru_utime.tv_sec >= timeLimit)
        // CPU time limit exceeded
        return false;


    // The current node of the search tree is consistent wrt to LAD and GAC(allDiff)
    // Save domain sizes and global all different matching
    // and search for the non matched vertex nextVar with smallest domain
    memcpy(nbVal, D->nbVal, Gp->nbVertices*sizeof(int));
    //memcpy(globalMatching, D->globalMatchingP, Gp->nbVertices*sizeof(int));
    nextVar=-1;
    int TMP[Gp->nbVertices + 1];
    for (u=0; u<Gp->nbVertices; u++){
        if ((nbVal[u]>1) &&
            ((nextVar<0) ||
             (nbVal[u]<nbVal[nextVar]) || // search variable with min domain
             ((nbVal[u] == nbVal[nextVar]) && (Gp->nbAdj[u]>Gp->nbAdj[nextVar])))) // break ties with degree
            nextVar=u;
    }
   //printf("%d\n",nextVar);
    if (nextVar==-1){// All vertices are matched => Solution found
        nbSol++;
        if (verbose >= 1){
            printf("Solution %d: ",nbSol);
            for (u=0; u<Gp->nbVertices; u++) printf("%d=%d ",u,D->val[D->firstVal[u]]);
            printf("\n");
        }
        //resetToFilter(D,Gp->nbVertices);
        if(nbSol >= upsol) {
        	printStats_large(false);
          exit(0);
            }
        return true;
    }
  
    // save the domain of nextVar to iterate on its values
     int *val = malloc(sizeof(int) * D->nbVal[nextVar]);
   // int val[D->nbVal[nextVar]];
    memcpy(val, &(D->val[D->firstVal[nextVar]]), D->nbVal[nextVar]*sizeof(int));

    int *tmpv = malloc(sizeof(int) * nbVal[nextVar]);
    for(i=0; i<nbVal[nextVar]; i++) tmpv[i] = val[i];
    quicks(tmpv, 0,nbVal[nextVar] - 1,Gt);
    // branch on nextVar=v, for every target node v in D(u)
 
    for(i=0; ((i<nbVal[nextVar]) && ((firstSol==0)||(nbSol==0))); i++){
        v = tmpv[i];
        if (verbose == 2) printf("Branch on %d=%d\n",nextVar,v);

        if ((!removeAllValuesButOnes_large(nextVar,v,D,Gp,Gt))  || (!matchVertex_large(nextVar,induced,D,Gp,Gt))){
            if (verbose == 2) printf("Inconsistency detected while matching %d to %d\n",nextVar,v);
            nbFail++;
            nbNodes++;
            //resetToFilter(D,Gp->nbVertices);
        }
        else if (!solve_large(timeLimit,firstSol,induced,verbose,D,Gp,Gt))
            // CPU time exceeded
            return false;
        // restore domain sizes and global all different matching
        if (verbose == 2) printf("End of branch %d=%d\n",nextVar,v);
        //memset(D->globalMatchingT,-1,sizeof(int)*Gt->nbVertices);
        memcpy(D->nbVal, nbVal, Gp->nbVertices*sizeof(int));
        
      
    }
    free(nbVal);
    free(val);
    free(tmpv);
    return true;
}


bool solve_presearch(int timeLimit, bool firstSol, bool induced, int verbose, Tdomain* D, Tgraph* Gp, Tgraph* Gt){
    // if firstSol then search for the first solution; otherwise search for all solutions
    // if induced then search for induced subgraphs; otherwise search for partial subgraphs
    // return false if CPU time limit exceeded before the search is completed
    // return true otherwise
    
    int u, v, nextVar, i;
    int* nbVal = malloc(sizeof(int) * Gp->nbVertices);
    //int nbVal[Gp->nbVertices];
    //int globalMatching[Gp->nbVertices];
    //int* globalMatching = malloc(sizeof(int) * Gp->nbVertices);
    nbNodes++;
  
    getrusage(RUSAGE_SELF, &ru);
    if (ru.ru_utime.tv_sec >= timeLimit)
        // CPU time limit exceeded
        return false;
    

    memcpy(nbVal, D->nbVal, Gp->nbVertices*sizeof(int));
    nextVar=-1;
    int TMP[Gp->nbVertices + 1];
    for (u=0; u<Gp->nbVertices; u++){
        if ((nbVal[u]>1) &&
            ((nextVar<0) ||
             (nbVal[u]<nbVal[nextVar]) || // search variable with min domain
             ((nbVal[u] == nbVal[nextVar]) && (Gp->nbAdj[u]>Gp->nbAdj[nextVar])))) // break ties with degree
            nextVar=u;
    }
   //printf("%d\n",nextVar);
    if (nextVar==-1){// All vertices are matched => Solution found
        nbSol++;
        if (verbose >= 1){
            printf("Solution %d: ",nbSol);
            for (u=0; u<Gp->nbVertices; u++) printf("%d=%d ",u,D->val[D->firstVal[u]]);
            printf("\n");
        }
        //resetToFilter(D,Gp->nbVertices);
        if(nbSol >= upsol) {
        	printStats_large(false);
          exit(0);
            }
        return true;
    }
     int *val = malloc(sizeof(int) * D->nbVal[nextVar]);
   // int val[D->nbVal[nextVar]];
    memcpy(val, &(D->val[D->firstVal[nextVar]]), D->nbVal[nextVar]*sizeof(int));

    int *tmpv = malloc(sizeof(int) * nbVal[nextVar]);
    for(i=0; i<nbVal[nextVar]; i++) tmpv[i] = val[i];
    //quicks(tmpv, 0,nbVal[nextVar] - 1,Gt);
    disorganise_large(tmpv,nbVal[nextVar] - 1);

    // branch on nextVar=v, for every target node v in D(u)
    for(i=0; ((i<nbVal[nextVar]) && ((firstSol==0)||(nbSol==0))); i++){
        v = tmpv[i];
        if (verbose == 2) printf("Branch on %d=%d\n",nextVar,v);
        if ((!removeAllValuesButOnes_large(nextVar,v,D,Gp,Gt))  || (!matchVertex_large(nextVar,induced,D,Gp,Gt) )){
            if (verbose == 2) printf("Inconsistency detected while matching %d to %d\n",nextVar,v);
            nbFail++;
            nbNodes++;
        }
        else if (!solve_presearch(timeLimit,firstSol,induced,verbose,D,Gp,Gt)){
            memcpy(D->nbVal, nbVal, Gp->nbVertices*sizeof(int));
            free(nbVal);
            free(val);
            free(tmpv);
            return false;
        }
            // CPU time exceeded
        
        // restore domain sizes and global all different matching
        if (verbose == 2) printf("End of branch %d=%d\n",nextVar,v);
        //memset(D->globalMatchingT,-1,sizeof(int)*Gt->nbVertices);
        memcpy(D->nbVal, nbVal, Gp->nbVertices*sizeof(int));
    }
    free(nbVal);
    free(val);
    free(tmpv);
    return true;
}




int solve_large_graph(int timeLimit, bool firstSol, int number_Sol, bool induced, int verbose, char *fileNameGp, char *fileNameGt){
	// Parameters
	
    int nbIsolatedP, nbIsolatedT;
	
	Tgraph *Gp = createGraph_large(fileNameGp,true,&nbIsolatedP);       // Pattern graph
	Tgraph *Gt = createGraph_large(fileNameGt,false,&nbIsolatedT);       // Target graph
	
	if (verbose >= 2){
		printf("Pattern graph:\n");
		printGraph_large(Gp);
		printf("Target graph:\n");
		printGraph_large(Gt);
	}
    if ((Gp->nbVertices+nbIsolatedP > Gt->nbVertices) || (Gp->maxDegree > Gt->maxDegree))
        return printStats_large(false);
	
	// Initialize domains
	Tdomain *D = createDomains_large(Gp, Gt);
	
	if (!initDomains_large(induced, D, Gp, Gt)) return printStats_large(false);
	
	if (verbose >= 2) printDomains_large(D, Gp->nbVertices);
	
	srand(1);
    upsol = number_Sol;
    // Math all vertices with singleton domains
    int u;
    int nbToMatch = 0;
    int toMatch[Gp->nbVertices];
    for (u=0; u<Gp->nbVertices; u++){
        D->globalMatchingT[D->globalMatchingP[u]] = u;
        if (D->nbVal[u] == 1)
            toMatch[nbToMatch++] = u;
    }
    if (!matchVertices_large(nbToMatch,toMatch,induced,D,Gp,Gt)){
        nbFail++;
        return printStats_large(false);
    }

	int tmp_time = timeLimit - 150;
	if(tmp_time < 0) tmp_time = 0;
    for(int i = 1;i <= 120;i++) {
        nbSol = 0;
        if(solve_presearch(timeLimit - tmp_time - (120 - i) * 1, firstSol,induced,verbose,D,Gp,Gt)) return printStats_large(false);
        
    }
	
    return printStats_large(!solve_large(timeLimit, firstSol, induced, verbose, D, Gp, Gt));
}
