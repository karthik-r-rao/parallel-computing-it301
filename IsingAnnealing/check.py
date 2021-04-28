print('\nStarting checker script...')

file1 = open("results.txt", "r")
file1 = file1.readlines()

vertices = []
s_init = []
s_final = []
p_init = []
p_final = []

for line in file1:
    line = line.split()
    if line[0][0] == 'S':
        s_init.append(float(line[3]))
        s_final.append(float(line[4]))
        vertices.append(int(line[1]))
    elif line[0][0] == 'P':
        p_init.append(float(line[3]))
        p_final.append(float(line[4]))

correct = 0
for i in range(len(vertices)):
    if s_init[i] == p_init[i] and s_final[i] == p_final[i]:
        correct+=1

print(f'{correct} out of {len(vertices)} are correct')