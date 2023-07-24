# Unstructured Mesh Partitioner - 2D

## Dependencies
To compiler the project you will need the following
compilers, libraries and tools

- clang or gcc compiler with C++17 support
- cmake
- boost
- metis
- vtk
- python3
- paraview

## Building
Perform the following steps to build the project:

```bash
$ mkdir -p ./build && cd ./build
$ cmake ..
$ build
```

## Building with Docker
`Dockerfile` contains all necessary installation steps for
dependencies. You can try to build, install and run the project
inside a Docker container if you are struggling to install
or configure some dependencies on your local machine.

You can find all necessary information about Docker
[here](https://docs.docker.com/engine/install/ubuntu/) and
[there](https://docs.docker.com/engine/install/linux-postinstall/).

The following commands will build an image and spin
the corresponding container in the interactive mode.

```bash
docker build -t ump2d:latest .
mkdir ./output
docker run -it --rm -v $(pwd)/output:/output ump2d:latest
```
*Note: the first image build will take some time.*

*Note: the last command also binds `output` directory on your host
with `/output` directory inside the container. Therefore, you
can store the program output in `/output` to make results to be
available on your host OS.*

*Note: files created inside the container will belong to 
`root` user. Therefore, you may need to use `chown` command
to get necessary permissions to access generated files.*

Now you can build the project:
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Running
Having compiled the project, you can run the partitioner. You will
need an unstructured 2D grid (in the `.vtk` format) as an input.

You can generate the mesh using `gmsh` software. `utils`
folder contains `naca-data2geo.py` script which can
convert airfoil profiles (given in NACA `.dat` format) to
`.geo` files required for `gmsh`.

Example:

```bash
cd build
./ump2d -i ../examples/example.vtk -o output/file.vtk -p 5
```

where `p` is the number of partitions.

Execute the following if you want to know more about all available
options:
```bash
./ump2d --help
```

## View Results
Use `Paraview` to examine the results of partitioning.

## Authors

- Ravil Dorozhinskii
