# How to run
    $ make
    $ ./huge_sort -l test_shuf -T tmp -p 10 -s test

# Notes
- Format of the input arguments can be found after run ./huge_sort -h
- Make sure the temporary directory has been existed
- Haven't implemented to delete middle results (temporary directory)
- Assume there is no limitation on the maximum number of opened file descriptors for single process
- Choose appropriate number of parts (-p) to run the huge sort, e.g., maximum number of allowed file descriptors, memory size
