
# Producer/Consumer

Here N producers and M consumers can be created each in its own thread.


<pre>
                                           |---|
                                        -->| C |
|---|                                  /   |---|
| P |--\                              /
|---|   \    |------------------|    /     |---|
         --->|    Container     |>-------->| C |
|---|   /    |------------------|    \     |---|
| P |--/                              \
|---|                                  \   |---|
                                        -->| C |
                                           |---|
</pre>



The `ProducerConsumer` class can be used as follows:

```c++

ProducerConsumer producerConsumer(2, 3, 100);

```

In the above example 2 producers and 3 consumers will created each in their own thread. 
Each producer will create 100 products that will be consumed by the consumers.



# Compile the code

From the repo root directory:
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```


## Compile in Debug/Release

By default the code will be compiled in Debug. 
In order to specify the build type, the CMake `CMAKE_BUILD_TYPE` flag should be defined. 

Debug:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Release:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```