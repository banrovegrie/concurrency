# REPORT ON TIME COMPARISION

### Expectation 
Concurrent mergesort would run faster because of shared memory.

### Result 
1. Simple mergesort is runs comparatively as fast as than concurrent mergesort (in some cases faster even).

2. Moreover, For big inputs, concurrent mergesort will not be able to run on normal computer because of excessive forking and will show the following error.
    
    ```bash
    pid: resource temporarily unavailable
    ```

For `n = 1000`:

**Concurrent Mergesort** takes `0.001255, 0.001051, 0.000901, 0.001246` secs for 4 different tries.

**Normal Mergesort** takes `0.001258, 0.001190, 0.001148, 0.001199` secs for 4 different tries respectively.

### Reason about above result
- Concurrent mergesort is accessing left array and right array simultaneously.

- When concurrent mergesort is accessing left array, left array is loaded into cache memory. Simultaneously, concurrent mergesort tries to access right array then cache miss would happen. Because of cache miss happens multiple times, execution of code slows down.

- Whereas in simple mergesort, 2 or 3 elements of left and right array is accessing at a time. therefore that elements can stay in cache and retrieval time of those elements is much lesser than retrieval time of elements in concurrent mergesort.

## Comparision with Threaded Mergesort
For `n = 1000`, I have observed clock values ranging from `0.000688` to `0.001722` secs.

### Reason
If the current process is multithreaded and more than one execution core is available, std::clock time may advance faster than wall clock.

## Conclusion
We can safely conclude that threaded mergesort is faster than the other two implementations of mergesort.