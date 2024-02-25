Multiprocessing in C

In this code, we generate random strings and use several concurrent processes to encrypt these strings in parallel and save the results to an outputfile.

To run the code, simply run 
```make && ./main <run_name> <num_strings>```

where <run_name> is replaced with a unique name for the current run to assure that the output files generated do not overwrite previous runs. <num_strings> is the number of strings to generate for encryption.
This will run the main program.

To run the test cases, simply run 
```make && ./test```


Stress testing notes:
The current implementation of this code only supports a max of 30,000 strings due to limitations in how many shared memory 
segments can be created on linux devices. This is becuase the code employes a queue in shared memory in which each node( containing a single string)
is its owns segment, and linux does not allow more than roughly 30,000 shared memory segments by default. 
In future work, it would be better to allocate multiple nodes for each segment, but that is
beyond the scope of this assignment as it states that a minimum of 10,000 words must be processable.
