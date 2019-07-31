# EnhancedFNCSApplicationPatch
This consists of the enhanced FNCS application patch. This enhancement enables it to implement applications such as internal real-time price, battery charging algorithm.


# Enhanced FNCS Tutorial
Framework for Network Co-Simulation (FNCS) is developed by Paicfic Northwest National Laboratory. The FNCS Tutorial can be found  https://github.com/GridOPTICS/FNCS-Tutorial/tree/master/demo-gld-ns3.  This enhanced FNCS tutorial basically explains a way to modify the FNCS application patch program to implement the smart grid applications.

This tutorial explains the steps for building and running FNCS, ns-3, and GridLAB-D. This enhancement is tested on the Linux operating system (Ubuntu 16.04; 64 bits).



# Software Installation
The installation process is same as explained in the FNCS Tutorial (https://github.com/GridOPTICS/FNCS-Tutorial/tree/master/demo-gld-ns3). The Following software need to be installed. ZeroMq, FNCS, Xerces, autoconf, automake, libtool, python, ns-3.

It is assumed that you will install all the software into $HOME/FNCS_install directory and you are using "bash" shell.
Set the environment variable
export FNCS_INSTALL="$HOME/FNCS_install"

# ZeroMQ Installation
ZeroMQ is a distributed messageing library. FNCS uses the ZeroMQ library. Use the following steps to get and install the ZeroMQ.

