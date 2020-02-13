import time as tm
import os

def exectime(cmd, generator):
    for _ in generator:
        start = tm.time()
        os.system(cmd + " {0} ".format(_))
        end = tm.time()
        yield end - start

os.system('gcc prac/serial_matrix.c -o a.out')
os.system('gcc -pthread prac/parallel_matrix.c -o b.out')
os.system('gcc ./prac/omp_matrix.c -lm -fopenmp -o c.out')

range_test = [10, 20, 30, 50, 100, 200, 500, 1000]

serial_list = exectime('./a.out', range_test)
parallel_list = exectime('./b.out', range_test)
omp_list = exectime('./c.out', range_test)

import matplotlib.pyplot as plt


plt.plot(range_test, list(serial_list), 'bo', 
         range_test, list(parallel_list), 'ro',
         range_test, list(omp_list), 'yo')
plt.xlabel('matrix size'); plt.ylabel('time')
plt.show()
