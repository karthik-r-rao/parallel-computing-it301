#include <iostream>
#include <vector>
#include <tuple>
#include <cstdlib>
#include <fstream>

using namespace std;

// add edge in a graph
void addEdge(vector<vector<tuple<int, int>>>& adj, int u, int v, int cost) {
    adj[u].push_back(make_tuple(v, cost));
    adj[v].push_back(make_tuple(u, cost));
}

// create a random undirected graph given number of vertices
void createGraph(vector<vector<tuple<int, int>>>& adj, int numVertices) {
    int numEdges = (rand() % (numVertices * (numVertices - 1) / 2 - (numVertices - 1))) + (numVertices - 1);
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

int main()
{
    int numVertices;
    cout << "Enter number of vertices in the graph: ";
    cin >> numVertices;
    vector<vector<tuple<int, int>>> adj(numVertices);
    createGraph(adj, numVertices);
    //printGraph(adj);

    // flatten graph to 1D array
    // flat will store graph as [dst1 weight1 dst2 weight2..]
    // places will store the indices in flat corresponding to src vertices
    vector<int> flat;
    vector<int> places;
    int count = 0;
    places.push_back(count);
    for (int i = 0; i < adj.size(); i++) {
        for (int j = 0; j < adj[i].size(); j++) {
            int x = get<0>(adj[i][j]);
            int y = get<1>(adj[i][j]);
            flat.push_back(x);
            flat.push_back(y);
            count+=2;
        }
        places.push_back(count);
    }
/*     for (int i = 0; i < flat.size(); i++) {
        cout << flat[i] << " ";
    }
    cout << "\nAnd count..\n";
    for (int i = 0; i < places.size(); i++) {
        cout << places[i] << " ";
    }
    cout << "\n"; */

    // write flat and places to txt files
    ofstream file1, file2;
    file1.open("flat.txt");
    cout << "Writing to flat.txt..." << endl;
    for (int i = 0; i < flat.size(); i++) {
        file1 << flat[i] << " ";
    }
    cout << "Finished writing to flat.txt" << endl;
    file1.close();
    file2.open("places.txt");
    cout << "Writing to places.txt..." << endl;
    for (int i = 0; i < places.size(); i++) {
        file2 << places[i] << " ";
    }
    cout << "Finished writing to places.txt" << endl;
    file2.close();
    return 0;
}