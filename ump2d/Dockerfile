FROM ubuntu:22.04

RUN apt-get update && \
apt-get install -yqq software-properties-common apt-utils build-essential wget git curl && \
apt-get install -yqq pkg-config cmake cmake-data tree vim && \
apt-get install -yqq python3 python3-dev python3-pip && \
add-apt-repository ppa:ubuntu-toolchain-r/test && \
apt-get install -yqq gcc-11 g++-11 gfortran-11 && \
apt-get install -yqq libboost-all-dev libmetis-dev gmsh && \
pip3 install --upgrade pip && \
pip3 install numpy>=1.12.0

# Insall VTK from sources
RUN apt-get install -yqq mesa-common-dev mesa-utils freeglut3-dev && \
wget -q https://www.vtk.org/files/release/9.2/VTK-9.2.2.tar.gz && \
tar -xvf ./VTK-9.2.2.tar.gz && \
mkdir -p ./VTK-9.2.2/build && cd ./VTK-9.2.2/build && \
cmake .. && make -j4 && make install && \
cd ../.. && rm -rf ./VTK-9.2.2*

COPY cmake          /ump2d/cmake
COPY src            /ump2d/src
COPY CMakeLists.txt /ump2d
COPY utils          /ump2d/utils
COPY examples       /ump2d/examples

WORKDIR /ump2d