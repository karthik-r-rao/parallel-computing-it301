#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>

const int SWEEPS = 100;

void printArray(int* arr, int size) {
	for (int i = 0; i < size; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

int getSize(FILE* fptr) {
	int size = 0;
	int temp;
	while (fscanf(fptr, "%d", &temp) == 1) {
		size++;
	}
	return size;
}

void getArray(FILE* fptr, int* arr) {
	int i = 0;
	while (fscanf(fptr, "%d", &arr[i]) == 1) {
		i++;
	}
}

// CUDA kernel for performing Ising Annealing in parallel
__global__ void isingAnnealingStep(int* d_flat, int* d_places, int* d_state, int* count, int vertices, int N){
    *count = N;
    int vertex = blockDim.x*blockIdx.x + threadIdx.x;
    if(vertex < vertices){
        int sigmaI = d_state[vertex];
        int H = 0;
        for (int i = d_places[vertex]; i < d_places[vertex + 1]; i += 2) {
            int sigmaJ = d_state[d_flat[i]];
            int J = d_flat[i + 1];
            H -= (J * sigmaI * sigmaJ);
        }
        __syncthreads();
        curandState_t random;
        curand_init(0, 0, 0, &random);
        int s = H / sigmaI;
        if (s > 0) {
            d_state[vertex] = -1;
        }
        else if (s < 0) {
            d_state[vertex] = 1;
        }
        else {
            d_state[vertex] = 1 - 2 * (curand(&random) % 2);
        }
        __syncthreads();
        curand_init(0, 0, 0, &random);
        if(*count > 0){
            int index = curand(&random) % vertices;
            if (d_state[index] == 1) {
                d_state[index] = -1;
            }
            else {
                d_state[index] = 1;
            }
            *count--;
        } 
    }
}

// utility function to compute Hamiltonian given a vertex
// ***not used in the parallel implementation***
int computeHamiltonian(int* flat, int* places, int* state, int vertex) {
	int sigmaI = state[vertex];
	int H = 0;
	for (int i = places[vertex]; i < places[vertex + 1]; i += 2) {
		int sigmaJ = state[flat[i]];
		int J = flat[i + 1];
		H -= (J * sigmaI * sigmaJ);
	}
	return H;
}


int main(){
	FILE* fptr;

	// load array $flat 
	fptr = fopen("flat.txt", "r");
	if (fptr == NULL)
	{
		printf("Error!");
		return -1;
	}
	// get number of integers for dynamic memory allocation on host
	int fsize = getSize(fptr);
	printf("Number of integers present in flat.txt: %d\n", fsize);

	// read values in text file to array $flat
	int* flat = (int*)malloc(sizeof(int) * fsize);
	fseek(fptr, 0, SEEK_SET);
	getArray(fptr, flat);

	//printArray(flat, fsize);
	fclose(fptr);

	/**********/

	// load array $places
	fptr = fopen("places.txt", "r");
	if (fptr == NULL)
	{
		printf("Error!");
		return -1;
	}
	// get number of integers for dynamic memory allocation on host
	int psize = getSize(fptr);
	printf("Number of integers present in places.txt: %d\n", psize);

	// read values in text file to array $places
	int* places = (int*)malloc(sizeof(int) * psize);
	fseek(fptr, 0, SEEK_SET);
	getArray(fptr, places);

	//printArray(places, psize);
	fclose(fptr);

	/**********/

	int vertices = psize - 1;
	int* state = (int*)malloc(sizeof(int) * vertices);
    printf("Number of vertices: %d\n", vertices);

	// initialize states randomly
	for (int i = 0; i < vertices; ++i) {
		state[i] = 1 - 2 * (rand() % 2);
	}

    double initial_energy = 0;
	for (int i = 0; i < vertices; i++) {
		int H = computeHamiltonian(flat, places, state, i);
		initial_energy += H;
	}
	printf("Initial energy: %f\n", initial_energy / 2);

    // allocate memory on device
    int* d_flat; 
    int* d_places;
    int* d_state;
    int* count;
    cudaMalloc((void**)&count, sizeof(int));
    cudaMalloc((void**)&d_flat, fsize * sizeof(int));
    cudaMalloc((void**)&d_places, psize*sizeof(int));
    cudaMalloc((void**)&d_state, vertices*sizeof(int));

    cudaMemcpy(d_flat, flat, fsize*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_places, places, psize*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_state, state, vertices*sizeof(int), cudaMemcpyHostToDevice);

    int threads = 1024;
    int blocks = (vertices / 1024) + 1;
    int N = vertices*0.75;
    clock_t begin = clock();
	for (int i = 0; i < SWEEPS; i++) {
        isingAnnealingStep<<<blocks, threads>>>(d_flat, d_places, d_state, count, vertices, N);
        //cudaDeviceSynchronize();
        N *= 0.9;
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    cudaMemcpy(flat, d_flat, fsize*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(places, d_places, psize*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(state, d_state, vertices*sizeof(int), cudaMemcpyDeviceToHost);

    double final_energy = 0;
	for (int i = 0; i < vertices; i++) {
		int H = computeHamiltonian(flat, places, state, i);
		final_energy += H;
	}
	printf("Final energy: %f\n", final_energy / 2);

    printf("Time taken for parallel Ising Annealing: %f seconds\n", time_spent);

    // store results in txt files
    // $time_spent $initial_energy $final_energy 
    printf("Storing parallel code's results in results.txt...\n");
    fptr = fopen("results.txt", "a");
    fprintf(fptr, "Parallel\t%d\t%f\t%f\t%f\n", vertices, time_spent, initial_energy, final_energy);
    fclose(fptr);
    printf("Finished writing to results.txt\n");

	// free dynamic memory on host and device
	free(flat);
	free(places);
	free(state);
    cudaFree(d_flat);
    cudaFree(d_places);
    cudaFree(d_state);
    
	return 0;
}