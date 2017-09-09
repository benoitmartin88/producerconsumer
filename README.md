
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


