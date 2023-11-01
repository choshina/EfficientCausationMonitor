# Efficient Online Causation Monitoring of Signal Temporal Logic
This repository is for the paper "Efficient Online Causation Monitoring of Signal Temporal Logic".

***

## System requirement

If you want to compile from source code, the requirement is as follows:

- C++ compiler.

- MATLAB (Simulink/Stateflow) version: >= 2020a. (MATLAB license needed)

Alternatively, you can also use the executables available in `bin/`.

## Installation of our tool:

- Clone the repository, via `https://github.com/choshina/STLCausationMonitor.git`

- Start MATLAB GUI, set up a C/C++ compiler using the command `mex -setup`. (Refer to [here](https://www.mathworks.com/help/matlab/matlab_external/changing-default-compiler.html) for more details.)
  > Running with MATLAB in shell is also OK.

- Navigate to the project home, and run `InstallBreach`
  > Note that we customized `InstallBreach`. It only compiles the online monitoring component. For the full functionality of Breach, please refer to [the original repository](https://github.com/decyphir/breach)

***

## Instructions on reproduction of experiment results

 ### Reproduce Figure 7a:
 - Run the script in MATLAB commandline: `Figure7a`

 ### Reproduce Figure 7b:
 - Run the script in MATLAB commandline: `Figure7b`

 ### Reproduce Table 1 and Table 2:

 Table 1 and Table 2 are based on the execution time of the **monitoring block** and the **Simulink model** as a whole, for each trace and each specification. Note the execution time is heavily dependent on the execution environment, e.g., the hardware condition, the OS, so we don't expect the users to exactly reproduce the data in the paper. Nevertheless, the comparison between different data entries, and the magnitude difference between data entries, should be roughly held as in the paper.

 - Run the script in MATLAB commandline: `Tables`
   - According to the prompt, select the specification you want to check
   - According to the prompt, input the number of repeatations for each trace
      > The data obtained in the table are based on 10 repeated executions for each trace and each specification. However, it can take quite some time to obtain the results if the repeatation number is set up to 10. Here, as we test, setting as 3 is fine.
   - During the running, the console displayes the running progress.
   - After it finishes, the console will display the corresponding row in Table 1, and the corresponding sub-table in Table 2.

 ***

 ## Usage of this tool on new problem instances

   ### New traces

   ### New specifications

   ### New Simulink models
