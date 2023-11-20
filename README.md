# Efficient Online Causation Monitoring of Signal Temporal Logic
This repository is for the reproduction of the experiment results in "Efficient Online Causation Monitoring of Signal Temporal Logic". The code is based on the online monitoring component of [Breach](https://github.com/decyphir/breach). 

***

## System requirement

If you want to compile from source code, the requirement is as follows:

- C++ compiler.

- MATLAB (Simulink/Stateflow) version: >= 2020a. (MATLAB license needed)

Alternatively, you can also use the executables available in `Online/bin/`.

## Installation:

- Clone the repository, via `https://github.com/choshina/EfficientCausationMonitor.git`

- Start MATLAB GUI, set up a C/C++ compiler using the command `mex -setup`. (Refer to [here](https://www.mathworks.com/help/matlab/matlab_external/changing-default-compiler.html) for more details.)
  > Running with MATLAB in shell is also OK.

- Navigate to the project home, and run `InstallBreach`
  > Note that we customized `InstallBreach`. It only compiles the online monitoring component. For the full functionality of Breach, please refer to [the original repository](https://github.com/decyphir/breach)

## Code Instruction:

- source code is in `Online/src` and `Online/include`
- executables are in `Online/bin`
- main function of robust online monitor [FMSD'17] is `Online/src/stl_eval_mex_pw.cpp`
- main function of causation monitor is `Online/src/stl_causation_opt.cpp`

***

## Reproduction of experiments in paper

- Navigate to `hscc24/`, all the traces and specifications have been stored at those `.mat` files
  
- In Line 3 of `exec.m`, specify the problem instance you want to run

- run `exec.m` in MATLAB.
