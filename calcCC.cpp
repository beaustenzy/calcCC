/* File:     calcCC.cpp
 *
 * Author:   Beau Stensgard
 *
 * Purpose:  Calculates the clustering coefficient of a graph using multithreading in C++.
 *
 * Compile:  g++ -O3 -w calcCC.cpp -lpthread -o calcCC
 *
 * To run:   ./calcCC filename threads
 *
 * Example:  ./calcCC toyGraph1.txt 2
 */
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>  /* atoi */
#include <iostream>
#include <fstream>
#include <vector>
#include "timer.h"
using namespace std;

double start, finish, elapsed;
typedef vector<vector<int> > AdjacencyMatrix;
AdjacencyMatrix adjMatrix;
AdjacencyMatrix adjList;
pthread_mutex_t myMutex;
long long n;
long thread_count;
double sum = 0.0;
double calcCC(int a) {
    int size = adjList[a].size();
    if (size < 2)
        return 0.0;
    int count = 0;
    for (int i = 0; i < size-1; i++) {
        int u = adjList[a][i];
        for (int j = i+1; j < size; j++) {
            int v = adjList[a][j];
            if (adjMatrix[u][v] == 1)
                count++;
        }
    }
    return (double) count/((size*(size-1))/2);
}
void *calcCCThread(void *rank) {
	long my_rank = (long) rank;
	long long i;
	long long my_n = (n + thread_count - 1) / thread_count;
	long long my_first_i = my_n*my_rank;
	long long my_last_i = min(my_first_i + my_n, n);
	double localsum = 0.0;
	for (int i=my_first_i; i<my_last_i; i++) { 
		localsum += calcCC(i);
	}
	pthread_mutex_lock(&myMutex);
	sum+= localsum;
	pthread_mutex_unlock(&myMutex);
}
void printAdjMatrix()
{
    for (int i=0; i<adjMatrix.size(); i++)
    {
        for (int j=0; j<adjMatrix[i].size(); j++) 
	    cout<<adjMatrix[i][j]<<" ";

        cout<<endl;
    }

}

void printAdjList()
{
    cout<<"Printing AdjList"<<endl;
    for (int i=0; i<adjList.size(); i++)
    {
        cout<<"Neighbors of node "<<i <<" :: ";
        for (int j=0; j<adjList[i].size(); j++) 
	    cout<<adjList[i][j]<<" ";

        cout<<endl;
    }

}
int main(int argc, char* argv[]) {
    if(argc<2){
      cout<<"To run: ./assign2Graph filename"<<endl;
      cout<<"./assign2Graph networkDatasets/toyGraph1.txt"<<endl;
      return 0;
    }
      
    fstream myfile(argv[1],std::ios_base::in);
	thread_count = strtol(argv[2], NULL, 10);
    int u,v;
    int maxNode = 0;
    vector<pair<int,int> > allEdges;
    while(myfile >> u >> v)
    {
        allEdges.push_back(make_pair(u,v));
        if(u > maxNode)
          maxNode = u;

        if(v > maxNode)
          maxNode = v;                 
    }

    n = maxNode +1;  //Since nodes starts with 0
    cout<<"Graph has "<< n <<" nodes"<<endl;

    adjMatrix = AdjacencyMatrix(n,vector<int>(n));
    adjList = AdjacencyMatrix(n,vector<int>());
    //populate the matrix
    for(int i =0; i<allEdges.size() ; i++){
       u = allEdges[i].first;
       v = allEdges[i].second;
       adjMatrix[u][v] = 1;
       adjMatrix[v][u] = 1;
       adjList[u].push_back(v);
       adjList[v].push_back(u);
    }
	pthread_mutex_init(&myMutex, NULL);
	pthread_t threads[thread_count];
	GET_TIME(start); 
	for (int i = 0; i < thread_count; i++)  
		pthread_create(&threads[i], NULL, calcCCThread, (void *)i);
	for (int i = 0; i < thread_count; i++) 
		pthread_join(threads[i], NULL);
	sum /= adjMatrix.size();
	GET_TIME(finish); 
	elapsed = finish - start;
	cout << "CC of graph: " << sum << endl;
	cout << "Total time elapsed: " << elapsed << " seconds." << endl;
	pthread_mutex_destroy(&myMutex);
 
return 0;
}
