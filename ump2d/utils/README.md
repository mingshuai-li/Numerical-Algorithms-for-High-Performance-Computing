### Let's get started

You will need to have a `dat` file which an arifoil geometry. Execute the following
to generate the corresponding `geo` file.

```
python3 ./naca-dat2geo.py -i <path to dat file> -o <output file name>.geo
```

Use `gmsh` to generate a mesh:
```
gmsh <geo file>.geo -2 -format vtk -o ./<output mesh name>.vtk
```