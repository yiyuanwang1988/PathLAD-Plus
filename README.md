# PathLAD+
SIP solver

This is a solver for solving subgraph isomorphism problems.

Our work is accepted by IJCAI2023 and our improvement is based on the PathLAD solver.



If you have segment faults, we recommend using the following commands before running these examples:

ulimit -s 40960 (or ulimit -s 102400)

After using this commands, if you still have segment faults, we also provide a code with modified data structures (PathLAD-Plus-modified) for machines with less memory.


We have further improved PathLAD+. The latest version of PathLAD+ is in PathLAD+v1.2, which provides better performance on small graphs while easily solving large graphs with millions of vertices.


Compilation environment: Ubuntu, gcc compiler, std=c99

