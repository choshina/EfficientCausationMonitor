#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "stl_driver.h"

#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  onlineMonitorWrapper

// Need to include simstruc.h for the definition of the SimStruct and
// its associated macro definitions.
#include "simstruc.h"
#include <chrono>

#define SIG_IDX 0
#define STL_IDX 1
#define MAX_ROB_IDX 2
#define REFRESH_IDX 3
//#define SUBFORM_IDX 4
#define DIAGNOSE_IDX 4

#define SIGNAL_STRING(S) ssGetSFcnParam(S,SIG_IDX)
#define STL_STRING(S)    ssGetSFcnParam(S,STL_IDX)
#define MAX_ROB(S)       ssGetSFcnParam(S,MAX_ROB_IDX)
#define REFRESH_RATE(S)  ssGetSFcnParam(S,REFRESH_IDX)
//#define SUBFORM(S)       ssGetSFcnParam(S,SUBFORM_IDX)
#define DIAGNOSE(S)      ssGetSFcnParam(S,DIAGNOSE_IDX)

#define NPARAMS 5

#define NOUTPORT 2
#define UP_IDX 0
#define LOW_IDX 1
//#define NMUP_IDX 2
//#define NMLOW_IDX 3


#if !defined(MATLAB_MEX_FILE)
/*
 * This file cannot be used directly with the Real-Time Workshop. However,
 * this S-function does work with the Real-Time Workshop via
 * the Target Language Compiler technology. See
 * matlabroot/toolbox/simulink/blocks/tlc_c/sfun_multiport.tlc
 * for the C version
 * matlabroot/toolbox/simulink/blocks/tlc_ada/sfun_multiport.tlc
 * for the Ada version
 */
# error This_file_can_be_used_only_during_simulation_inside_Simulink
#endif

using namespace std;
using namespace CPSGrader;


/*====================*
 * S-function methods *
 *====================*/

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
/* Function: mdlCheckParameters =============================================
 * Abstract:
 *    Validate our parameters to verify they are okay.
 */
static void mdlCheckParameters(SimStruct *S)
{
    
    if (mxIsEmpty(    ssGetSFcnParam(S,SIG_IDX)) ||
            mxIsSparse(   ssGetSFcnParam(S,SIG_IDX)) ||
            mxIsComplex(  ssGetSFcnParam(S,SIG_IDX)) ||
            mxIsLogical(  ssGetSFcnParam(S,SIG_IDX)) ||
            mxIsNumeric(  ssGetSFcnParam(S,SIG_IDX)) ||
            mxIsDouble(   ssGetSFcnParam(S,SIG_IDX)) ||
            !mxIsChar( ssGetSFcnParam(S,SIG_IDX))) {
        ssSetErrorStatus(S,"Parameter signal names must be a string.");
        return;
    }
    if (mxIsEmpty(    ssGetSFcnParam(S,STL_IDX)) ||
            mxIsSparse(   ssGetSFcnParam(S,STL_IDX)) ||
            mxIsComplex(  ssGetSFcnParam(S,STL_IDX)) ||
            mxIsLogical(  ssGetSFcnParam(S,STL_IDX)) ||
            mxIsNumeric(  ssGetSFcnParam(S,STL_IDX)) ||
            mxIsDouble(   ssGetSFcnParam(S,STL_IDX)) ||
            !mxIsChar( ssGetSFcnParam(S,STL_IDX))) {
        ssSetErrorStatus(S,"Parameter formula must be a string.");
        return;
    }
    
    if (mxIsEmpty(    ssGetSFcnParam(S,MAX_ROB_IDX)) ||
            mxIsSparse(   ssGetSFcnParam(S,MAX_ROB_IDX)) ||
            mxIsComplex(  ssGetSFcnParam(S,MAX_ROB_IDX)) ||
            mxIsLogical(  ssGetSFcnParam(S,MAX_ROB_IDX)) ||
            !mxIsNumeric(  ssGetSFcnParam(S,MAX_ROB_IDX)) ||
            !mxIsDouble(   ssGetSFcnParam(S,MAX_ROB_IDX)) ||
            mxIsChar( ssGetSFcnParam(S,MAX_ROB_IDX))) {
        ssSetErrorStatus(S,"Parameter max. robustness must be a positive double.");
        return;
    }

        if (mxIsEmpty(    ssGetSFcnParam(S,REFRESH_IDX)) ||
            mxIsSparse(   ssGetSFcnParam(S,REFRESH_IDX)) ||
            mxIsComplex(  ssGetSFcnParam(S,REFRESH_IDX)) ||
            mxIsLogical(  ssGetSFcnParam(S,REFRESH_IDX)) ||
            !mxIsNumeric(  ssGetSFcnParam(S,REFRESH_IDX)) ||
            !mxIsDouble(   ssGetSFcnParam(S,REFRESH_IDX)) ||
            mxIsChar( ssGetSFcnParam(S,REFRESH_IDX))) {
        ssSetErrorStatus(S,"Parameter refresh rate must be a positive double.");
        return;
        }

//         if (mxIsEmpty(    ssGetSFcnParam(S,SUBFORM_IDX)) ||
//             mxIsSparse(   ssGetSFcnParam(S,SUBFORM_IDX)) ||
//             mxIsComplex(  ssGetSFcnParam(S,SUBFORM_IDX)) ||
//             mxIsLogical(  ssGetSFcnParam(S,SUBFORM_IDX)) ||
//             mxIsNumeric(  ssGetSFcnParam(S,SUBFORM_IDX)) ||
//             mxIsDouble(   ssGetSFcnParam(S,SUBFORM_IDX)) ||
//             !mxIsChar( ssGetSFcnParam(S,SUBFORM_IDX))) {
//         ssSetErrorStatus(S,"Parameter sub-form must be a string.");
//         return;
//         }

        if (mxIsEmpty(    ssGetSFcnParam(S,DIAGNOSE_IDX)) ||
            mxIsSparse(   ssGetSFcnParam(S,DIAGNOSE_IDX)) ||
            mxIsComplex(  ssGetSFcnParam(S,DIAGNOSE_IDX)) ||
            mxIsLogical(  ssGetSFcnParam(S,DIAGNOSE_IDX)) ||
            !mxIsNumeric(  ssGetSFcnParam(S,DIAGNOSE_IDX)) ||
            !mxIsDouble(   ssGetSFcnParam(S,DIAGNOSE_IDX)) ||
            mxIsChar( ssGetSFcnParam(S,DIAGNOSE_IDX))) {
        ssSetErrorStatus(S,"Parameter diagnose must be a positive double.");
        return;
        }
    

    
    char *signal_buf = mxArrayToString(SIGNAL_STRING(S));
    char *stl_buf = mxArrayToString(STL_STRING(S));
   
    string phi_st = "signal "+ string(signal_buf) + "\n" + "phi:=" + string(stl_buf);
    mxFree(signal_buf);
    mxFree(stl_buf);
    
    STLDriver *stl_driver = new STLDriver();
    bool parse_success = stl_driver->parse_string(phi_st);
    if (parse_success==0)
        ssSetErrorStatus(S,"Error while parsing STL Formula.");
    delete stl_driver;
    
}
#endif /* MDL_CHECK_PARAMETERS */

// Function: mdlInitializeSizes ===============================================
// Abstract:
//    The sizes information is used by Simulink to determine the S-function
//    block's characteristics (number of inputs, outputs, states, etc.).
static void mdlInitializeSizes(SimStruct *S)
{
    //ssSetUserData(S, (void*) mexGetVariablePtr("base", "elapse")); // Set a pointer to a MATLAB workspace variable
    
    
    // Check the number of expected parameters
    ssSetNumSFcnParams(S, NPARAMS);
    // Parameter mismatch will be reported by Simulink
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }
  
    ssSetNumDiscStates(S, 2);
    
    mdlCheckParameters(S);
    if (ssGetErrorStatus(S) != NULL) {
        return;
    }
    ssSetSFcnParamTunable(S,STL_IDX,true);
    ssSetSFcnParamTunable(S,SIG_IDX,true);
    ssSetSFcnParamTunable(S,MAX_ROB_IDX,true);
    ssSetSFcnParamTunable(S,REFRESH_IDX,true);
    //ssSetSFcnParamTunable(S,SUBFORM_IDX,true);
    ssSetSFcnParamTunable(S,DIAGNOSE_IDX,true);

    // Specify I/O
    if (!ssSetNumInputPorts(S, 1)) return;
    if(!ssSetInputPortDimensionInfo(S, 0, DYNAMIC_DIMENSION)) return;
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    
    if (!ssSetNumOutputPorts(S,1)) return;
    ssSetOutputPortWidth(S, 0, NOUTPORT);
    
    ssSetNumSampleTimes(S, 1);
    
    // Reserve place for C++ object
    ssSetNumPWork(S, 1);
    
    ssSetSimStateCompliance(S, USE_CUSTOM_SIM_STATE);
    
    ssSetOptions(S,
            SS_OPTION_WORKS_WITH_CODE_REUSE |
            SS_OPTION_EXCEPTION_FREE_CODE);
    
}
#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_DIMENSION_INFO
static void mdlSetInputPortDimensionInfo(SimStruct *S,
        int_T port, const DimsInfo_T *dimsInfo)
{
    if(!ssSetInputPortDimensionInfo(S, port, dimsInfo)) return;
}
#endif

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_OUTPUT_PORT_DIMENSION_INFO
static void mdlSetOutputPortDimensionInfo(SimStruct *S,
        int_T port, const DimsInfo_T *dimsInfo)
{
    if(!ssSetOutputPortDimensionInfo(S, port, dimsInfo)) return;
}
#endif

#if defined(MATLAB_MEX_FILE)
# define MDL_SET_DEFAULT_PORT_DIMENSION_INFO
/* Function: mdlSetDefaultPortDimensionInfo ===========================================
 * Abstract:
 *   In case no ports were specified, the default is an input port of width 2
 *   and an output port of width 1.
 */
static void mdlSetDefaultPortDimensionInfo(SimStruct *S)
{
    ssSetInputPortWidth(S, 0, 1);
    ssSetOutputPortWidth(S, 0, NOUTPORT);
}
#endif

#define MDL_INITIALIZE_CONDITIONS
/* Function: mdlInitializeConditions ========================================
 * Abstract:
 *    Initialize discrete states to initial min-max robustnesses
 */
static void mdlInitializeConditions(SimStruct *S)
{
    real_T *x0     = ssGetRealDiscStates(S);
    real_T max_rob = mxGetScalar(MAX_ROB(S));

    x0[UP_IDX]  =  max_rob;
    x0[LOW_IDX] = -max_rob;
    //x0[NMUP_IDX] = max_rob;
    //x0[NMLOW_IDX] = -max_rob;
}


// Function: mdlInitializeSampleTimes =========================================
// Abstract:
//   This function is used to specify the sample time(s) for your
//   S-function. You must register the same number of sample times as
//   specified in ssSetNumSampleTimes.
static void mdlInitializeSampleTimes(SimStruct *S)
{
    //ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    real_T refresh_rate = mxGetScalar(REFRESH_RATE(S));
    ssSetSampleTime(S, 0, refresh_rate);
    ssSetOffsetTime(S, 0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
    
}

// Function: mdlStart =======================================================
// Abstract:
//   This function is called once at start of model execution. If you
//   have states that should be initialized once, this is the place
//   to do it.
#define MDL_START
static void mdlStart(SimStruct *S)
{
    
    char *signal_buf = mxArrayToString(SIGNAL_STRING(S));
    char *stl_buf = mxArrayToString(STL_STRING(S));   

    //char *sf_buf = mxArrayToString(SUBFORM(S));
    real_T diag = mxGetScalar(DIAGNOSE(S));

    string phi_st = "signal "+ string(signal_buf) + "\n" + "phi:=" + string(stl_buf);
    
    mxFree(signal_buf);
    mxFree(stl_buf);
    
    // Store new C++ object in the pointers vector
    STLDriver *stl_driver  = new STLDriver();
    
    stl_driver->parse_string(phi_st);

    //stl_driver->set_sub_form(string(sf_buf));
    stl_driver->set_diagnose(diag);

    //cout<<string(sf_buf)<<endl;
    //cout<<diag<<endl;
    //cout<<string(sf_buf)<<endl;

    //mxFree(sf_buf);

    ssGetPWork(S)[0] = stl_driver;

}

#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid) {
  
    STLDriver *stl_driver = static_cast<STLDriver *>(ssGetPWork(S)[0]);
    double max_rob = mxGetScalar(MAX_ROB(S));
    Signal::BigM = max_rob;
    Signal::MaxLength = 10000;
    Signal::RefreshRate = 0.1;
    
    // Get data addresses of I/O
    InputRealPtrsType  u        =  ssGetInputPortRealSignalPtrs(S,0);
    int_T              u_width  =  ssGetInputPortWidth(S,0);
    real_T           *xd        =  ssGetRealDiscStates(S);
    real_T             T        =  ssGetT(S);


    //start_time
    auto start = std::chrono::high_resolution_clock::now();

    //make data precise to handle numerical error
    double rT = (double)T;
    rT = (int)(rT*10 + 0.5); //refresh rate = 0.1
    rT = (double)rT/10;
    double rrT = rT;
    rT = rT - stl_driver->sum_shift; // calibrate the time stamp

    //handle numerical issue
    if(rT == 0){
        stl_driver->old_qnmono_up = Signal::BigM;
        stl_driver->old_qnmono_low = - Signal::BigM;
    }

    vector<double> points;
    //points.push_back(T);
    points.push_back(rT);
    for(int_T i = 0; i< u_width; ++i ) {
        points.push_back(*u[i]);
    }
    
    stl_driver->data.push_back(points);
    
    //classic monitoring results
    double rob_up = max_rob;
    double rob_low= -max_rob;

    //Boolean non-monotone results
    bool b_nmono_up = true;
    bool b_nmono_low = false;

    //quantitative non-monotone results
    double q_nmono_up = max_rob;
    double q_nmono_low = -max_rob;

    Signal z_up, z_low;
     
    //int vio_epoch = 0;
    //int sat_epoch = 0;


    if (stl_driver->data.size()>=2) {
        transducer *phi = stl_driver->formula_map["phi"]->clone();
        phi->set_trace_data_ptr(stl_driver->data);

        phi->set_duration();
        //phi->set_selected_subformula(stl_driver->sub_form);
        
        rob_up  = phi->compute_upper_rob();

        rob_low = phi->compute_lower_rob();

        vector<double> vio_set;
        vector<double> sat_set;

//== from here, the code from EMSOFT'22
//
//         if(stl_driver->diagnose != 0){
//             if(rob_up<0){
//                 phi->collect_vio_epoch(vio_set, phi->start_time);
//                 //the commented code is from EMSOFT'22, we now separate the functions set_epoch and should_reset           
//                 //if (stl_driver->set_epoch(vio_set)){ 
//                 if (stl_driver-> should_reset(vio_set)){
//                     double delta = phi->min_shift_vio(phi->start_time);
//                     stl_driver->reset_monitor(delta);
//                 }else{
//                     stl_driver-> set_epoch(vio_set);
//                 }
// 
//             }else if(rob_low > 0){
//                 phi->collect_sat_epoch(sat_set, phi->start_time);
//                 //the commented code is from EMSOFT'22, we now separate the functions set_epoch and should_reset
//                 //if (stl_driver->set_epoch(sat_set)){
//                 if(stl_driver-> should_reset(sat_set))
//                     double delta = phi->min_shift_sat(phi->start_time);
//                     stl_driver->reset_monitor(delta);
//                 }else{
//                     stl_driver-> set_epoch(sat_set);
//                 }
//             }
//         }
//== EMSOFT'22 code ends here



//== from here, the code of CAV'23
//
        if(stl_driver->diagnose  == 1){
            // collect epoch only
            if(rob_up<0){
                phi->collect_vio_epoch(vio_set, phi->start_time, rT);
                stl_driver->set_epoch(vio_set);
            }else if(rob_low > 0){
                phi->collect_sat_epoch(sat_set, phi->start_time, rT);
                stl_driver->set_epoch(sat_set);
            }
        }else if(stl_driver -> diagnose == 2){
            // collect epoch and reset
            if(rob_up < 0){
                phi->collect_vio_epoch(vio_set, phi->start_time, rT);
//                 if(rrT == 8.6 || rrT == 8.7 || rrT == 8.8){
//                     cout<< "rT" << rT <<" rrT : " << rrT << " ";
//                     for(auto i = vio_set.begin(); i!=vio_set.end(); i ++){
//                         cout<<(*i)<< " ";
//                     }
//                     cout<<endl;                
//                 }
                if(stl_driver-> should_reset(vio_set, rT)){
                    
                    double delta = phi->min_shift_vio(phi->start_time);
                    //delta = 12.1;
//                     if(rT >15 &&rT<15.4){
//                        auto ii = phi->trace_data_ptr->end();
//                         auto jj = (*ii).begin();
//                        cout<< " before : " << stl_driver->data.size() << "rT" << rT << " " << (*jj)  <<endl;
//                         
//                     }
                    stl_driver->reset_monitor(delta);
//                     if(rT >15 &&rT<15.4){
//                        //cout<<"rT: " << rT << " yes "  << rob_up << " delta: "<< delta <<endl;
//                        //cout<< stl_driver->data.size() <<endl;
//                        cout << "rT " << rT << " rob: " <<phi->compute_upper_rob() << "===finish===" <<endl;
//                     }
//                     if(rrT == 15.3){
//                         cout<< "rrt15.3 " <<stl_driver->data.size() << " rob: " <<phi->compute_upper_rob()<<endl;
//                     }
                    //cout <<"rrT: " <<rrT <<" delta: " << delta << endl;
                    //
                }else{
                    stl_driver-> set_epoch(vio_set);
                }
            }else if(rob_low > 0){
                phi->collect_sat_epoch(sat_set, phi->start_time, rT);
                if (stl_driver->should_reset(sat_set, rT)){
                    double delta = phi->min_shift_sat(phi->start_time);
                    stl_driver->reset_monitor(delta);
                
                }else{
                    stl_driver-> set_epoch(sat_set);
                }
            }
        }else if(stl_driver -> diagnose == 3){
            // collect epoch and compute Boolean non-monotone semantics
            if(rob_up < 0){
                phi->collect_vio_epoch(vio_set, phi-> start_time, rT);
                if(stl_driver->epoch_increase(vio_set, rT)){
                    b_nmono_up = false;
                    stl_driver -> set_epoch(vio_set);
                }
//                 if(rT == 10 || rT == 10.1){
//                     cout<<"rT: "<<rT <<" b_nmono_up: " << b_nmono_up<<endl;
//                     for(auto i = vio_set.begin(); i!=vio_set.end(); i ++){
//                         cout<<(*i)<< " ";
//                     }
//                     cout<<endl;
//                 }
                
            }else if(rob_low > 0){
                phi->collect_sat_epoch(sat_set, phi-> start_time, rT);
                if(stl_driver->epoch_increase(sat_set, rT)){
                    b_nmono_low = true;
                    stl_driver -> set_epoch(sat_set);
                }
            }
        }else if(stl_driver -> diagnose == 4){
            // compute quantitative non-monotone semantics according to CAV'23
            // use one more parameter t to indicate the current time, rather than creat new formula class
            
            // report q_nmono_up
            q_nmono_up = phi->compute_qnmono_upper(phi-> start_time, rT);
            if(q_nmono_up == Signal::BigM){ //to handle numerical issue
                q_nmono_up = stl_driver -> old_qnmono_up;
            }
            stl_driver->old_qnmono_up = q_nmono_up;

            //cout<<"rT: " <<rT <<"upper: " <<q_nmono_up <<endl;

            // report q_nmono_low
            q_nmono_low = phi->compute_qnmono_lower(phi-> start_time, rT);
            if(q_nmono_low == -Signal::BigM){ //to handle numerical issue
                q_nmono_low = stl_driver -> old_qnmono_low;
            }
            stl_driver -> old_qnmono_low = q_nmono_low;

            //cout<<"debug: " << q_nmono_up << " " <<endl;
        }else if(stl_driver -> diagnose == 5){
            // a more efficient version of Mode 4
            // with the heuristics for alw and ev temporal operators
            q_nmono_up = phi->improved_qnmono_upper(phi-> start_time, rT);

            q_nmono_low = phi->improved_qnmono_lower(phi-> start_time, rT);
        }
//== CAV'23 code ends here


       //if(stl_driver->data.back().front() == 16){
//         cout<<flag<<endl;
//         if(flag == 1){
// //             for(auto j = vio_set.begin(); j!= vio_set.end(); j++){
// //                 cout<<(*j)<<endl;
// //             }
        //     stl_driver->print_trace();
         //}

        //TODO: print the epoch at the last moment.

        delete phi;
    }  
   
    auto stop = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<microseconds> diff = stop - start;
    //auto duration = duration_cast<microseconds>(stop - start);
    std::chrono::duration<double, std::milli> diff = stop - start;
    stl_driver->update_time(diff.count());


    if (rob_up==TOP)
        rob_up= max_rob;
    if (rob_low==BOTTOM)
        rob_low= -max_rob;

    //cout<<rob_up<<rob_low<<endl;
    
    xd[UP_IDX] =  rob_up;
    xd[LOW_IDX] = rob_low;
    if(stl_driver->diagnose == 3){
        //xd[UP_IDX] = b_nmono_up? max_rob:-max_rob;
        //xd[LOW_IDX] = b_nmono_low? -max_rob:max_rob;
        if(!b_nmono_up){
            xd[UP_IDX] = -1;
            xd[LOW_IDX] = -1;
        }else if(b_nmono_low){
            xd[UP_IDX] = 1;
            xd[LOW_IDX] = 1;
        }else{
            xd[UP_IDX] = 0;
            xd[LOW_IDX] = 0;
        }
    }else if(stl_driver->diagnose == 4){
        xd[UP_IDX] = q_nmono_up;
        xd[LOW_IDX] = q_nmono_low;
    }

    //cout<<rT<<" " <<b_nmono_up<<" "<<b_nmono_low<<endl;
    //cout<< "data:" <<xd[UP_IDX] <<" " <<xd[LOW_IDX] << " "<<xd[NMUP_IDX] <<" "<<xd[NMLOW_IDX]<<endl;
    //cout<< "rT: " << rT <<endl;
}



// Function: mdlOutputs =======================================================
// Abstract:
//   compute the outputs of your S-function block.
static void mdlOutputs(SimStruct *S, int_T tid)
{
 
    real_T            *xd    = ssGetRealDiscStates(S);
    real_T            *y     = ssGetOutputPortRealSignal(S, 0);
       
    y[UP_IDX] =  xd[UP_IDX];
    y[LOW_IDX] = xd[LOW_IDX];
    //y[NMUP_IDX] = xd[NMUP_IDX];
    //y[NMLOW_IDX] = xd[NMLOW_IDX];
}

// Function: mdlTerminate =====================================================
// Abstract:
//   In this function, you should perform any actions that are necessary
//   at the termination of a simulation.  For example, if memory was
//   allocated in mdlStart, this is the place to free it.
static void mdlTerminate(SimStruct *S)
{
    // Retrieve and destroy C++ object

    STLDriver *stl_driver = static_cast<STLDriver *>(ssGetPWork(S)[0]);
    //cout<< "reset nums: " << stl_driver->num_reset <<endl;
    //cout<< "block time cost: " << stl_driver->elapse_time <<endl;
    //cout<< stl_driver->elapse_time << " ";
    ofstream writefile;
    writefile.open("log.txt", ios::app);
    writefile << stl_driver->elapse_time << endl;
    writefile.close();
    delete stl_driver;
}


// Required S-function trailer
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
