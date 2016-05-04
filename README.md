Superconductor
==============

A simulation tool for superconducting circuits.

As a part of my diploma thesis (http://www.andreas-dewes.de/en/publications), I developed a simulation framework for the modeling and analysis of superconducting circuits. The main objective of this project was to develop a software package incorporating the following features:

* Simulation of arbitrary superconducting, electrical circuits containing both “classical” elements (e.g. resistors, capacitors) as well as superconducting components (e.g. Josephson junctions)
* Providing an open source alternative to other simulation packages available for the simulation of superconducting circuits (see e.g. [PSS91], [PSK+97] or [Whi91])
* Remedying the limitations of certain other simulation packages for circuit simulations (e.g. restriction of the maximum number of elements connected to a single node)
 
## Building It

To build it under Windows, use the Visual Studio project file in the `circuit` directory.

To build it under Linux, simply use g++ (sorry for the code quality, this is from my Academic work :D):

```
g++ -w *.cpp cline/sample.cpp -lpthread -DCIRCUIT_SIM=1 -o csim
```

## Running a Simulation

To run a script, invoke the `csim` binary like this:

```
csim -f scripts/dc_squid.ini
```

This should start the simulation of a DC squid and store the results in a subfolder of the `data` directory (make sure that this exists). Make sure to install `gnuplot` if you want the plotting commands to perform properly.
 
##Overview

The software comes with a custom-built scripting language that allows the user to control all aspects of the circuit simulation and generation of data.

After describing the circuit topology using the scripting language, the software automatically generates a full spanning tree of the circuit and calculates a complete set of node and mesh equations for the circuit.
The solution of this set of equations is done by using a modified biconjugate gradient descent method which linearizes the circuit around the current solution and calculates the state of it after a given time increment \Delta t to a chosen numerical precision. 
The main advantage of this method is that it requires only multiplication of sparse matrices (which can be implemented efficiently) and not the LU decomposition of the circuit matrix, which is computationally much more expensive.

##Example

The following script illustrates the generation of an IV curve for a superconducting DC SQUID circuit:

```C
CIRC_initialize();
CIRC_STEPT=0.1;
add_JJ("JJ_right",0,1,1.0,0.6);
add_PS("Flux",0,2,0.0);
add_JJ("JJ_left",2,3,1.0,2.1);
add_L("L_right",1,4,1*1.56);
add_L("L_left",3,4,1*1.56);
add_R("R",4,5,0.01);
add_CS("current",5,0,2.0);
CIRC_build_trees();
t=new_buffer();
graph(t);
set_flush(t,0);
IVAMP=4.6;
ICSTEPS=100;
ICAMP=PI*2.0;
INTT=800.0;

for(x=1.9516;x<=2.6;x=x+0.05)
{
  current_I=x;
  IV("JJ_left","Flux",t);
}
```

##Terms of Usage

The code (except the numerical recipes snippets that were used and the Lua interface library code) can be freely reused. Feel free to include a reference to this code if you reuse it in your own project.

##Further Reading

For more details on the simulation software, please check out my MSc thesis in Physics at http://www.andreas-dewes.de/en/publications/

##References

* [PSS91] S. Polonsky, V. Semenov, and P. Shevchenko. Pscan: personal superconductor circuit analyser. Supercond. Sci. Technol., 4:667–670, 1991.
* [PSK+97] S. Polonsky, S. Shevchenko, P. Kirichenko, et al. Pscan’96: new software for simulation and optimization of complex rsfq circuits. IEEE Trans. Appl. Supercond., 7(2):2685–2689, 1997.
* [Whi91] S. Whitely. Josephson junctions in spice3. IEEE Trans. on Magn., 27(2):2902–2905, 1991.
