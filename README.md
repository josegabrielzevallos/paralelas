# Cparalela
## RUN MPI
~~~
$ mpicc file.c -o app
$ mpirun -np 3 (para 3 procesadores) ./app
~~~

## RUN PTHREAD
~~~
$ gcc -o app file.c -lpthread (file es cualquier programa de la carpeta src)
$ ./app 5 ->(para cinco threads)
~~~

~~~
$ gcc -o app file -lpthread -rand.c  (si tiene #include "rand.h")
$ ./app 4
~~~

## RUN OPENMP
~~~
$ gcc -o app file.c -fopenmp (file es cualquier programa de la carpeta src)
$ ./app 4 100 g
~~~
