#include <iostream>
#include <vector>
#include <tuple>
#include <cstdlib>

using namespace std;

int SWEEPS = 100;
int numVertices = 100000;

// add edge in a graph
void addEdge(vector<vector<tuple<int, int>>>& adj, int u, int v, int cost) {
    adj[u].push_back(make_tuple(v, cost));
    adj[v].push_back(make_tuple(u, cost));
}

// create a random undirected graph given number of vertices
void createGraph(vector<vector<tuple<int, int>>> &adj) {
    int numEdges = (rand() % (numVertices*(numVertices-1)/2 - (numVertices-1))) + (numVertices-1);  
    for (int i = 0; i < numEdges; i++) {
        int u = rand() % numVertices;
        int v = rand() % numVertices;
        int cost = rand() % 10 + 5;
        addEdge(adj, u, v, cost);
    }
}

// print adjacency list of graph
void printGraph(vector<vector<tuple<int, int>>>& adj) {
    cout << "Printing adjacency list..\n";
    for (int i = 0; i < adj.size(); i++) {
        cout << i << ":";
        for (int j = 0; j < adj[i].size(); j++) {
            printf("(%d, %d) ", get<0>(adj[i][j]), get<1>(adj[i][j]));
        }
        cout << "\n";
    }
}

// Ising Annealing
void isingAnnealing(vector<vector<tuple<int, int>>>& adj, vector<int> &state) {
    int N = numVertices / 2;    // number of states to randomly flip in each sweep
    for (int i = 0; i < SWEEPS; i++) {
        for (int j = 0; j < numVertices; j++) {
            int H = 0;  // Hamiltonian for each vertex
            int sigmaI = state[j];  // sigma(i) in the paper
            for (int k = 0; k < adj[j].size(); k++) {
                int sigmaJ = state[get<0>(adj[j][k])];  // sigma(j) in the paper
                int J = get<1>(adj[j][k]);  // J(i, j) in the paper
                H -= (J*sigmaI*sigmaJ); // no external bias in Hamiltonian equation 
            }
            // local update
            int s = H / sigmaI;
            if (s > 0) {
                state[j] = -1;
            }
            else if(s < 0){
                state[j] = 1;
            }
            else {
                state[j] = 1 - 2 * (rand() % 2);
            }
        }
        // randomly flip N vertices
        for (int j = 0; j < N; j++) {
            int index = rand() % N;
            if (state[index] == 1) {
                state[index] = -1;
            }
            else {
                state[index] = 1;
            }
        }
        N*=0.75;    // annealing schedule
    }
}

int main()
{
    vector<vector<tuple<int, int>>> adj(numVertices);
    createGraph(adj);
    //printGraph(adj);
    vector<int> state(numVertices); // keeps track of state of each vertex
    // initialise states randomly
    for (int i = 0; i < numVertices; i++) {
        state[i] = 1 - 2 * (rand() % 2);
    }
    isingAnnealing(adj, state);
    int cnt = 0;    // number of 1's
    for (int i = 0; i < state.size(); i++) {
        if (state[i] == 1) {
            cnt++;
        }
    }
    cout << cnt;
    cout << "\n";
    return 0;
}