# symmetry-factors

This script calculates symmetry factors of real scalar fields with 3-point and 4-point vertices.
It actually generates all permutations of the graphs of Feynman diagrams associated with perturbative
term of the correlation function. As a sanity check, the total number of graphs should match the expected
number of `(NUM_FIELDS-1)!!`.

To use, edit the `constexpr` parameters in the top of the file and recompile and run with
```
$ make
$ ./sym_factors
```

You can search for a specific diagram at the end of the main function by specifying the list of
edges in the diagram. You can also try to calculate only one diagram which is faster, but I've checked its
correctness less.

*WARNING* Don't use for anything super accurate: I don't make any claims about correctness or being bug-free!
