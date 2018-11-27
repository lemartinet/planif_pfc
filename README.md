This repository contains code to simulate and investigate an artificial neural network architecture inspired by the interactions of the hippocampus and the prefrontal cortex, two brain regions that are known to play key roles in spatial navigation, decision making and action planning.

The different versions of the code that you can find in the commits and tags/branches were used in the papers:
* Spatial learning and action planning in a prefrontal cortical network model (2011), LE Martinet, D Sheynikhovich, K Benchenane, A Arleo, PLoS Comput Biol 7 (5), e1002045.
* A cortical column model for multiscale spatial planning (2010), LE Martinet, A Arleo, From Animals to Animats 11, 347-358.
* Map-based spatial navigation: A cortical column model for action planning (2008), LE Martinet, JB Passot, B Fouque, JA Meyer, A Arleo, International Conference on Spatial Cognition, 39-55.
* Modelling the cortical columnar organisation for topological state-space representation, and action planning (2008), LE Martinet, B Fouque, JB Passot, JA Meyer, A Arleo, International Conference on Simulation of Adaptive Behavior, 137-147.

The simulation code is based on the Webots robotic platform and is implemented in C++.
The data analyses use Python and Matlab to investigate both simulated neuron activity and real neuron recordings from rats performing navigation tasks.

The general usage is to call launcher.py
```
python launcher.py [-c] [-r "first last laby"] [-a "first last"] 
-c: compile the code
-r: run the simulations from "first" to "last" using the "laby" maze (tolman_maze/tolman_maze_big)
-a: extract data for the raw outputs from "first" to "last"
```
