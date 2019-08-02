# EnhancedFNCSApplicationPatch
This consists of the enhanced FNCS application patch. This enhancement enables it to implement applications such as internal real-time price, battery charging algorithm.


# Enhanced FNCS Tutorial
Framework for Network Co-Simulation (FNCS) is developed by Pacific Northwest National Laboratory. The FNCS Tutorial can be found at  https://github.com/GridOPTICS/FNCS-Tutorial/tree/master/demo-gld-ns3.  This enhanced FNCS tutorial basically explains a way to modify, build, and execute the FNCS application patch program to implement the smart grid applications.

This tutorial explains the steps for building and running FNCS, ns-3, and GridLAB-D. This enhancement is tested on the Linux operating system (Ubuntu 16.04; 64 bits).



# Software Installation
The installation process is same as explained in the FNCS Tutorial (https://github.com/GridOPTICS/FNCS-Tutorial/tree/master/demo-gld-ns3). ZeroMq, FNCS, Xerces, autoconf, automake, libtool, python, ns-3 softwares are need to be installed. 

It is assumed that you will install all the softwares into $HOME/FNCS_install directory and you are using "bash" shell.
Set the environment variable
export FNCS_INSTALL="$HOME/FNCS_install"

# ZeroMQ Installation
ZeroMQ is a distributed messageing library. FNCS uses the ZeroMQ library. Use the following steps to get and install the ZeroMQ.

```
# we are doing everything from your $HOME directory
cd $HOME

# download zeromq
wget http://download.zeromq.org/zeromq-3.2.4.tar.gz
# if you do not have wget, use
# curl -O http://download.zeromq.org/zeromq-3.2.4.tar.gz

# unpack zeromq, change to its directory
tar -xzf zeromq-3.2.4.tar.gz
cd zeromq-3.2.4

# configure, make, and make install 
./configure --prefix=$FNCS_INSTALL
make
make install
```
# CZMQ Installation
```
# we are doing everything from your $HOME directory
cd $HOME

# download czmq
wget http://download.zeromq.org/czmq-3.0.0-rc1.tar.gz
# if you do not have wget, use
# curl -O http://download.zeromq.org/czmq-3.0.0-rc1.tar.gz

# unpack czmq, change to its directory
tar -xzf czmq-3.0.0-rc1.tar.gz
cd czmq-3.0.0

# configure, make, and make install
./configure --prefix=$HOME/FNCS_install --with-libzmq=$HOME/FNCS_install
make
make install
```

# FNCS Installation
FNCS is a broker which synchronizes the GridLAB-D and ns-3 simulator. It exchanges messages between the GridLAB-D and ns-3.
```
# we are doing everything from your $HOME directory
cd $HOME

# download FNCS
git clone https://github.com/FNCS/fncs

# change to FNCS directory
cd fncs

# configure, make, and make install
./configure --prefix=$FNCS_INSTALL --with-zmq=$FNCS_INSTALL
make
make install

```
# Xerces-c++ Installation
```
# we are doing everything from your $HOME directory
cd $HOME

# download Xerces-C++ 3.1.1 source code
wget http://apache.mirrors.pair.com//xerces/c/3/sources/xerces-c-3.1.1.tar.gz
# if there is any problem then try this
wget https://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.1.1.tar.gz
# if you do not have wget, use
# curl -O http://apache.mirrors.pair.com//xerces/c/3/sources/xerces-c-3.1.1.tar.gz

# unpack xerces, change to its directory
tar -xzf xerces-c-3.1.1.tar.gz
cd xerces-c-3.1.1

# configure, make, and make install 
./configure --prefix=$FNCS_INSTALL
make
make install
```
# GridLAB-D Installation
GridLAB-D is a power distribution system simulator and analysis tool. Install GridLAB-D using following steps:

```
# we are doing everything from your $HOME directory
cd $HOME

# download our FNCS-capable version of GridLAB-D
git clone https://github.com/gridlab-d/gridlab-d

# change to FNCS-gridlab-d directory
cd gridlab-d

# checkout the develop branch
#git checkout -b develop origin/develop

# run to autotools to generate the configure script and Makefile.in
# templates
# minimum required versions:
# autoconf 2.63
# automake 1.11
# libtool 2.2.6b
autoreconf -fi

# configure, make, and make install
./configure --prefix=$FNCS_INSTALL --with-xerces=$FNCS_INSTALL --with-fncs=$FNCS_INSTALL --enable-silent-rules 'CFLAGS=-g -O0 -w' 'CXXFLAGS=-g -O0 -w' 'LDFLAGS=-g -O0 -w'
make
make install
```

# ns-3 Installation
ns-3 is an event-driven network simulator. There is an FNCS application patch that is added to the ns-3 simulator. This application patch receives FNCS messages from GridLAB-D and injects them into the network. This tuorial explains the modified FNCS application patch. This modified patch implements the internal real-time pricing and battery charging algorithm.
To see the modifications, read the file fncs-application.cc
Use the following steps to install ns-3:
```
# we are doing everything from your $HOME directory
cd $HOME

# download our FNCS version of ns-3 which consists of enhanced FNCS application patch
git clone https://github.com/BrijendraMNNIT/EnhancedFNCSApplicationPatch.git

cd EnhancedFNCSApplicationPatch
# we begin our install from the ns-3.26 directory
cd ns-3.26

# the ns-3 install typically uses the compiler flag for
# warnings-as-errors which often broke our ability to build and install
# it, so we recommend the following configure of ns-3
CFLAGS="-g -O2" CXXFLAGS="-g -O2" ./waf configure --prefix=$FNCS_INSTALL --with-fncs=$FNCS_INSTALL --with-zmq=$FNCS_INSTALL --disable-python

# 'make'
./waf build

# insatll
./waf install
```

# Important Environment Variables

After installing all the softwares, we need to set environment variables. These setting can be done through the env.sh file which consists of the following
```
#!/bin/sh

export FNCS_INSTALL="$HOME/FNCS_install"

# update LD_LIBRARY_PATH
if test "x$LD_LIBRARY_PATH" = x
then
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$FNCS_INSTALL/lib64"
else
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$FNCS_INSTALL/lib64:$LD_LIBRARY_PATH"
fi

# update PATH
if test "x$PATH" = x
then
    export PATH="$FNCS_INSTALL/bin"
else
    export PATH="$FNCS_INSTALL/bin:$PATH"
fi

export FNCS_FATAL=yes
```
# Description of How FNCS Application Patch Is Enhanced 
It would be easy to understand the modification by looking the inheritance diagram and the state chart diagram. The inheritance diagram of the FNCSApplication class is given below. State chart diagram for the two home instances is depicted. 
Inheritance diagram before modification ![Alt text](inheritencediagram1-eps-converted-to-1.png?raw=true "Inheritance diagram before modification")

Inheritance diagram after modification ![Alt text](inheritencediagram2-eps-converted-to-1.png?raw=true "Inheritance diagram after modification")

State chart diagram before modification ![Alt text](state1new.png?raw=true "State chart diagram before modification")

State chart diagram after modification ![Alt text](state2new.png?raw=true "State chart diagram after modification")


# GridLAB-D Model
Set up of the topology ![Alt text](fig5-1.png?raw=true "Simulation topology")
The GridLAB-D model has six houses. These houses act on the basis of the internal real-time price and their batteries charging and discharging are controlled by an application program. 

# ns-3 Model
The file firstN.cc creates one more node than the number of houses in the GridLAB-D. This extra node implements the internal real-time pricing algorithm and charging/discharging algorithms
"This strategy can be used for implementing other mechanisms of internal real-time pricing, battery charging algorithms, or some other new applications."

# Running the Co-Simulation
```
Go into the directory GridLAB-DAndns-3Model which is inside the directory EnhancedFNCSApplicationPatch.
Open three terminals 
      1. At first terminal
            $ source env.sh
            $ chmod +x compile-ns3.sh
            $ ./compile-ns3.sh firstN.cc  
            $ ./firstN
      2. At the second terminal
            $ source env.sh
            $ gridlabd ./microgridversion5.glm
      3. At the third terminal
            $ source env.sh
            $  fncs_broker 2
```
Here, the simulation set up executes for seven days simulation time. After finishing seven days simulation days it tries for 8th day and  gives message the input file not found. This means that simulation is finished. Now, You can analyse your results.
