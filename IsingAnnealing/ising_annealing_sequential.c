#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void isingAnnealing(int* flat, int* places, int* state, int vertices) {
	int N = vertices*0.75;
	int* tmp_state = (int*)malloc(sizeof(int) * vertices);
	// copy $state to $tmp_state
	for (int i = 0; i < vertices; i++) {
		tmp_state[i] = state[i];
	}
	for (int i = 0; i < SWEEPS; i++) {
		for (int j = 0; j < vertices; j++) {
			int sigmaI = state[j];
			int H = computeHamiltonian(flat, places, state, j);

			// local update
			int s = H / sigmaI;
			if (s > 0) {
				tmp_state[j] = -1;
			}
			else if (s < 0) {
				tmp_state[j] = 1;
			}
			else {
				tmp_state[j] = 1 - 2 * (rand() % 2);
			}
		}
		// randomly flip N vertices
		for (int j = 0; j < N; j++) {
			int index = rand() % vertices;
			if (tmp_state[index] == 1) {
				tmp_state[index] = -1;
			}
			else {
				tmp_state[index] = 1;
			}
		}

		// copy $tmp_state to $state
		for (int j = 0; j < vertices; j++) {
			state[j] = tmp_state[j];
		}
		N *= 0.9;    // annealing schedule
	}
	free(tmp_state);
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
	// get number of integers for dynamic memory allocation
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
	// get number of integers for dynamic memory allocation
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

	// initialize states randomly
	for (int i = 0; i < vertices; ++i) {
		state[i] = 1 - 2 * (rand() % 2);
	}

	// compute initial energy
	double initial_energy = 0;
	for (int i = 0; i < vertices; i++) {
		int H = computeHamiltonian(flat, places, state, i);
		initial_energy += H;
	}
	printf("Initial energy: %f\n", initial_energy / 2);

	// perform ising annealing
	clock_t begin = clock();
	isingAnnealing(flat, places, state, vertices);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	double final_energy = 0;
	for (int i = 0; i < vertices; i++) {
		int H = computeHamiltonian(flat, places, state, i);
		final_energy += H;
	}
	printf("Final energy: %f\n", final_energy / 2);

	printf("Time taken for sequential Ising Annealing: %f seconds\n", time_spent);

	// append time taken to times.txt
	fptr = fopen("times.txt", "a");
	if (fptr == NULL)
	{
		printf("Error!");
		return -1;
	}
	fprintf(fptr, "%f\n", time_spent);
	fclose(fptr);

	// free dynamic memory
	free(flat);
	free(places);
	free(state);

	return 0;
}