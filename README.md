# Efficient Online Causation Monitoring of Signal Temporal Logic
This repository is for the paper "Efficient Online Causation Monitoring of Signal Temporal Logic".

***

## System requirement

If you want to compile from source code, the requirement is as follows:

- C++ compiler.

- MATLAB (Simulink/Stateflow) version: >= 2020a. (MATLAB license needed)

Alternatively, you can also use the executables available in `bin/`.

## Installation of our tool:

- Clone the repository, via `https://github.com/choshina/EfficientCausationMonitor.git`

- Start MATLAB GUI, set up a C/C++ compiler using the command `mex -setup`. (Refer to [here](https://www.mathworks.com/help/matlab/matlab_external/changing-default-compiler.html) for more details.)
  > Running with MATLAB in shell is also OK.

- Navigate to the project home, and run `InstallBreach`
  > Note that we customized `InstallBreach`. It only compiles the online monitoring component. For the full functionality of Breach, please refer to [the original repository](https://github.com/decyphir/breach)

***
