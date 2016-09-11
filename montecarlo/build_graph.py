import matplotlib.pyplot as plt
import numpy

file = open('out.dat', 'r')
x = []
y = []
for line in file:
    splitted = line.split(':')
    x.append(float(splitted[0]))
    y.append(float(splitted[1]))
y = [y[0] / y[i] for i in range(len(y))]
x = x[1:]
y = y[1:]
fig = plt.figure()
ax = fig.gca()
ax.set_xticks(numpy.arange(1, 12, 1))
plt.ylabel('acceleration')
plt.xlabel('threads')
plt.title('Multithreading advantage example')
plt.plot(x, y)
plt.grid()
plt.savefig('plot.png')
