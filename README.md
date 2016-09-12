# Clingo3to4
This is a wrapper for the F2LP preprocessor which is a part of cplus2asp Toolchain. 
F2LP outputs code in older ASP syntax which is not compatible with ASP Core 2 specifications. The output of F2LP code cannot be run on the newest iterstion of Clingo i.e. Clingo 4 (vaialble here https://sourceforge.net/projects/potassco/files/clingo/). This wrapper takes in F2LP ouput as it's input and outputs a Logic Program file with syntax that is compatible with ASP Core 2 specifications and can be used as an input for Clingo4. This module is also being used in the Cplus2ASP toolchain.  
