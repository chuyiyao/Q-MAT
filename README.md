# Q-MAT（Unfinished）
This is a C++ implementation of the 2016 siggraph paper *"Q-MAT: Computing Medial Axis Transform by Quadradic Error Minimization"*, aiming to simplify the medial axis tranform(MAT).

# Basic Programme Frame

![ProgramFrame](https://github.com/chuyiyao/Q-MAT/blob/master/Images/program_frame.png)
![ProgramFrame_eng](https://github.com/chuyiyao/Q-MAT/blob/master/Images/frame_eng.png)

At first I intend to use the library *CGAL* as the basic Mesh data structure for its *halfedge* structure, which speeds up the processing of find every adjacent face of a vertex. However, it turns out that this kind of structure only suits the surface mesh. The initial Medial Axis Transform is much more complex for there could be three or more triangle meshes share one edge.

Finally, I decide to partly use the library [*trimesh*](http://gfx.cs.princeton.edu/proj/trimesh2/) as our base class, we define a derived class adding new data structures to storing the edge and other edge's and vertex's properties.

# reference

The whole work is based on
```bib
@article{Li:2015:QCM:2870647.2753755,
 author = {Li, Pan and Wang, Bin and Sun, Feng and Guo, Xiaohu and Zhang, Caiming and Wang, Wenping},
 title = {Q-MAT: Computing Medial Axis Transform By Quadratic Error Minimization},
 journal = {ACM Trans. Graph.},
 issue_date = {December 2015},
 volume = {35},
 number = {1},
 month = dec,
 year = {2015},
 issn = {0730-0301},
 pages = {8:1--8:16},
 articleno = {8},
 numpages = {16},
 url = {http://doi.acm.org/10.1145/2753755},
 doi = {10.1145/2753755},
 acmid = {2753755},
 publisher = {ACM},
 address = {New York, NY, USA},
 keywords = {Medial axis, quadratic error metric, simplification, stability ratio, volume approximation},
} 
```
