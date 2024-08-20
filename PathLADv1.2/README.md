SIP solver (https://github.com/yiyuanwang1988/PathLAD-Plus)

This is a solver for solving subgraph isomorphism problems, our improvement is based on the PathLAD solver.

Origin PathLAD  has been written by Christine Solnon (see http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html for more details)


## Compiling

To build, type 'make'. You will need a GCC compiler.

## Running

./main -p pattern.txt -t target.txt -f -s 3600
 
Files
-----

Makefile: make an executable (main) and test on 2 subgraph isomorphism instances 

pattern.txt: example of non labelled pattern graph

target.txt: example of target graph

README: this file

Usage
-----

-p FILE  Input pattern graph (mandatory)

-t FILE  Input target graph (mandatory)

-s INT   Set CPU time limit in seconds (default: 60)

-f       Stop at first solution (default: search for all solutions)

-n INT If -f is not used, -n represents the maximum number of solutions to be searched (default: 100000)

-v       Print solutions (default: only number of solutions)

-h       Print this help message

Graph format
------------

Pattern and target graphs should be defined in two text files. The format of these files is defined as follows:
- The first line only contains the number n of vertices.
- Each following line gives, for each vertex i, the number of successors of i followed by the list of successors of i.
- Nodes must be numbered from 0 and, if there is an edge from i to j and an edge from j to i, then j must be a successor of i and i must be a successor of j.

