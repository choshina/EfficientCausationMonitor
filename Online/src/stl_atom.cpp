#include "stdafx.h"
#include <transducer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "tools.h"
#include <exception>
#include "signal_expr.h"
#ifdef IS_PC
#include <minmax.h>
#endif

namespace CPSGrader {

    constant_transducer::constant_transducer(const string &p) {

        value = 0.;
        if (!s_to_d(p, value))  param = p;

    }

    double stl_atom::compute_robustness() {
#ifdef DEBUG__
        printf(">> stl_atom::compute_robustness:              IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
#endif
        // Assumes childL and childR have signals with same number of samples
        // TODO should be eventually improved with proper operations on signals
        childL->compute_robustness();
        childR->compute_robustness();
        auto itL = childL->z.begin();
        auto itR = childR->z.begin();
        for (; itL != childL->z.end() &&  itR != childR->z.end() ;itL++, itR++) {
            double t = (*itL).time;
            double vL = (*itL).value;
            double vR = (*itR).value;
            double dL = (*itL).derivative;
            double dR = (*itR).derivative;
		
            if (comp == comparator::LESSTHAN )
                z.appendSample(t, vR-vL, dR-dL);
            else
                z.appendSample(t, vL-vR, dL-dR);	
        }
        z.endTime = childL->z.endTime;



#ifdef DEBUG__
        cout << "OUT z:" << z << endl;
        printf("<< stl_atom::compute_robustness:              OUT.\n");
#endif

        return z.front().value;
    }



    double stl_atom::compute_qnmono_upper(double tau, double b){
//         if(tau == 30 && b == 30){
//             //cout <<" check: " << get_z(tau, 1) << z <<endl;
//             set_horizon(30, 30);
//             double r = compute_robustness();
//             cout<<"r: " << r <<endl;
//             cout <<" check: " << get_z(tau, 1) << z <<endl;
//         }
        if(tau == b){
            compute_robustness();
            auto it = z.begin();
            double cau = Signal::BigM;
            for(;it!= z.end();it++){
                if((*it).time == tau){
                    cau = (*it).value;
                }
            }
            return cau;
        }else{
            return Signal::BigM;
        }
    }

    double stl_atom::compute_qnmono_lower(double tau, double b){
        if(tau == b){
            compute_robustness();
            auto it = z.begin();
            double cau = Signal::BigM;
            for(;it!= z.end();it++){
                if((*it).time == tau){
                    cau = (*it).value;
                }
            }
            return cau;
        }else{
            return - Signal::BigM;
        }
    }

    double stl_atom::compute_causation_mid_upper(double tau, double b){
//         z.clear();
//         if(tau == b){
//             set_horizon(tau, tau);
//             double r = compute_robustness();
//             return r;
//         }else{
//             return Signal::BigM;
//         }
        if(tau == b){
            compute_robustness();
            auto it = z.begin();
            double cau = Signal::BigM;
            for(;it!= z.end();it++){
                if((*it).time == tau){
                    cau = (*it).value;
                }
            }
            return cau;
        }else{
            return Signal::BigM;
        }
    }

    double stl_atom::compute_causation_mid_lower(double tau, double b){
//         z.clear();
//         if(tau == b){
//             set_horizon(tau, tau);
//             return compute_robustness();
//         }else{
//             return - Signal::BigM;
//         }
        if(tau == b){
            compute_robustness();
            auto it = z.begin();
            double cau = Signal::BigM;
            for(;it!= z.end();it++){
                if((*it).time == tau){
                    cau = (*it).value;
                }
            }
            return cau;
        }else{
            return - Signal::BigM;
        }
    }

//     double stl_atom::compute_causation_opt_upper(double tau, double b){
//         if(tau == b){
//             compute_robustness();
//             auto it = z.begin();
//             double cau = Signal::BigM;
//             for(;it!= z.end();it++){
//                 if((*it).time == tau){
//                     cau = (*it).value;
//                 }
//             }
//             return cau;
//         }else{
//             return Signal::BigM;
//         }
//     }

//     double stl_atom::compute_causation_opt_lower(double tau, double b){
//         if(tau == b){
//             compute_robustness();
//             auto it = z.begin();
//             double cau = - Signal::BigM;
//             for(;it!= z.end();it++){
//                 if((*it).time == tau){
//                     cau = (*it).value;
//                 }
//             }
//             return cau;
//         }else{
//             return - Signal::BigM;
//         }
//     }

    double stl_atom::causation_opt_upper(){
        int size = trace_data_ptr->size();
        double b = (trace_data_ptr->at(size-1))[0];
        cau_up.reset();
        if(double_less(b, start_time)){
            cau_up.appendSample(start_time, TOP);
        }else if(double_greater(b, end_time)){
            cau_up.appendSample(start_time, TOP);
            cau_up.appendSample(end_time, TOP);
        }else{
            cau_up.appendSample(start_time, TOP);
            if(size > 2){
                double b_ = (trace_data_ptr->at(size-2))[0];
                cau_up.appendSample(b_, TOP);
            }
            int i = childL->get_index();
            double vL = (trace_data_ptr->at(size-1))[i];

            double vR = childR->get_value();
            if (comp == comparator::LESSTHAN )
                cau_up.appendSample(b, vR-vL);
            else
                cau_up.appendSample(b, vL-vR);
        }

        return cau_up.front().value;
    }

    double stl_atom::causation_opt_lower(){
        int size = trace_data_ptr->size();
        double b = (trace_data_ptr->at(size-1))[0];
        cau_low.reset();
        if(double_less(b, start_time)){
            cau_low.appendSample(start_time, BOTTOM);
        }else if(double_greater(b, end_time)){
            cau_low.appendSample(start_time, BOTTOM);
            cau_low.appendSample(end_time, BOTTOM);
        }else{
            cau_low.appendSample(start_time, BOTTOM);
            if(size > 2){
                double b_ = (trace_data_ptr->at(size-2))[0];
                cau_low.appendSample(b_, BOTTOM);
            }
            int i = childL->get_index();
            double vL = (trace_data_ptr->at(size-1))[i];

            double vR = childR->get_value();
            if (comp == comparator::LESSTHAN )
                cau_low.appendSample(b, vR-vL);
            else
                cau_low.appendSample(b, vL-vR);
        }
        return cau_low.front().value;
    }


    
    double signal_transducer::compute_robustness() {
#ifdef DEBUG__
        printf(">> signal_transducer::compute_robustness:     IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
#endif


        short i = signal_map[variable];
        //double t0;  // now, we assume t0 = 0.; WARNING: THIS WILL BREAK CPSGRADER... 
        double t=0., v=0.;  
        double res= 0.;
            
        if (i) {
            while ( (td_idx <= trace_data_ptr->size()-1)  && t<=end_time)   {
                t = (trace_data_ptr->at(td_idx))[0];
                v = (trace_data_ptr->at(td_idx))[i];
                // cout << "t: " << t << " v: " << v << endl;
                z.appendSample(t, v);
                td_idx++;
			}
		}

        if (t>end_time)  
            z.endTime = end_time;
        
        if (z.endTime>start_time)
            z.resize(start_time, max(start_time,z.endTime),v);  // if we reached start_time trim everything before
        else
            z.resize(z.endTime, z.endTime, v);   // otherwise, only keep the last value seen before start_time

        res= z.front().value;    
#ifdef DEBUG__
        printf("<< signal_transducer::compute_robustness      OUT.\n");
#endif
        return res;
    }


    double constant_transducer::compute_robustness() {

#ifdef DEBUG__
        printf(">> constant_transducer::compute_robustness:IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
#endif
        
        // TODO exception if parameter not found for some reason ? For now, we get 0. as a default silent value.
        if (!param.empty()) {
            get_param(param, value);
        }

        double t=0.;  
            
        while ((td_idx <= trace_data_ptr->size()-1) && t<=end_time)   {
            t = (trace_data_ptr->at(td_idx))[0];
            z.appendSample(t, value);
            td_idx++;
        }
		
        if (t>end_time)  
            z.endTime = end_time;
        
        if (z.endTime>start_time) {
            z.resize(start_time, max(start_time,z.endTime),value);  // if we reached start_time trim everything before
        }
        else {
            z.resize(z.endTime, z.endTime, value);   // otherwise, only keep the last value seen before start_time
        }
#ifdef DEBUG__
        printf("<< constant_transducer::compute_robustness OUT.\n");
#endif
        return value;
    }
}

