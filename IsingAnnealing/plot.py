import matplotlib.pyplot as plt

file1 = open("results.txt", "r")
file1 = file1.readlines()

vertices = []
s_times = []

p_times = []

for line in file1:
    line = line.split()
    if line[0][0] == 'S':
        s_times.append(float(line[2]))
        vertices.append(int(line[1]))
    elif line[0][0] == 'P':
        p_times.append(float(line[2]))

plt.scatter(vertices, s_times, label="sequential program")
plt.scatter(vertices, p_times, label="parallel program")
plt.title('Time taken for max-cut via Ising Annealing')
plt.legend()
plt.xlabel('Number of vertices')
plt.ylabel('Time taken in seconds')
plt.show()