#include "stdafx.h"
#include <transducer.h>
#include <algorithm>
#include <math.h>

namespace CPSGrader {

    /* Interval robustness */

    // default (static): interval robustness is the same as normal robustness, with TOPs and BOTTOMs at
    // the ends 
    double transducer::compute_lower_rob(){
#ifdef DEBUG__
        printf(">  transducer:computer_lower_rob              IN.\n");
#endif
        compute_robustness();
        if (z.endTime < start_time) {
            z_low.appendSample(start_time, BOTTOM); 
        }
        else
            z_low = z;

         
        //if(trace_data_ptr->back().front() == 28){
        //    cout<<z<<endl;
        //}
#ifdef DEBUG__
        printf( "<  transducer:computer_lower_rob              OUT.\n");
#endif
        return z_low.front().value;
    };

    double transducer::compute_upper_rob(){
#ifdef DEBUG__
        printf( ">  transducer:computer_upper_rob              IN.\n");
#endif
        compute_robustness();
        if (z.endTime < start_time) {
            z_up.appendSample(start_time, TOP); 
        }
        else
            z_up = z;
#ifdef DEBUG__
        printf( "<  transducer:computer_upper_rob              OUT.\n");
#endif
        return z_up.front().value;
    };

    
    double and_transducer::compute_lower_rob(){
#ifdef DEBUG__
        printf( ">  and_transducer:computer_lower_rob           IN.\n");
#endif
        childL->compute_lower_rob();  
        childR->compute_lower_rob();
        z_low.compute_and(childL->z_low,childR->z_low);
        z_low.resize(start_time, min(childL->z_low.endTime,childR->z_low.endTime),BOTTOM);
        if (z_low.empty())
            z_low.appendSample(start_time, BOTTOM);
#ifdef DEBUG__
        printf( "<  and_transducer:computer_lower_rob           OUT.\n");
#endif
        return z_low.front().value;
    };


    double and_transducer::compute_upper_rob(){
#ifdef DEBUG__
        printf( ">  and_transducer:computer_upper_rob           IN.\n");
#endif
        childL->compute_upper_rob();
        childR->compute_upper_rob();
        z_up.compute_and(childL->z_up,childR->z_up);
        z_up.resize(start_time,z_up.endTime,TOP);
        if (z_up.empty())
            z_up.appendSample(start_time,TOP);
#ifdef DEBUG__
        printf( "<  and_transducer:computer_upper_rob           OUT.\n");
#endif
        return z_up.front().value;
    };


    double and_transducer::compute_qnmono_upper(double tau, double b){
        double x = childL->compute_qnmono_upper(tau, b);
        double y = childR->compute_qnmono_upper(tau, b);
        return x<y?x:y;
    }

    double and_transducer::compute_qnmono_lower(double tau, double b){
        double x1 = childL->compute_qnmono_lower(tau, b);
        
        childR->compute_lower_rob();
        auto itR = childR->z_low.begin();
        double x2 = -Signal::BigM;
        for(;itR!=childR->z_low.end();itR++){
            if((*itR).time == tau){
                x2 = (*itR).value;
            }
        }
//         childR->z_low.clear();
//         childR->set_horizon(tau, tau);
//         double x2 = childR->compute_lower_rob();
        //double x2 = childR->get_zlow(tau);
        double x = x1<x2?x1:x2;

        //double y1 = childL->get_zlow(tau);
//         childL->z_low.clear();
//         childL->set_horizon(tau, tau);
//         double y1 = childL->compute_lower_rob();
        childL->compute_lower_rob();
        auto itL = childL->z_low.begin();
        double y1 = -Signal::BigM;
        for(;itL!= childL->z_low.end(); itL++){
            if((*itL).time == tau){
                y1 = (*itL).value;
            }
        }
        
        double y2 = childR->compute_qnmono_lower(tau, b);
        double y = y1<y2?y1:y2;

        return x>y?x:y;
    }

    double and_transducer::compute_causation_mid_upper(double tau, double b){
        double x = childL->compute_causation_mid_upper(tau, b);
        double y = childR->compute_causation_mid_upper(tau, b);
        return x<y?x:y;
    }

    double and_transducer::compute_causation_mid_lower(double tau, double b){
        double x1 = childL->compute_causation_mid_lower(tau, b);
        childR->compute_lower_rob();
        auto itR = childR->z_low.begin();
        double x2 = -Signal::BigM;
        for(;itR!=childR->z_low.end();itR++){
            if((*itR).time == tau){
                x2 = (*itR).value;
            }
        }
        double x = x1<x2?x1:x2;

        childL->compute_lower_rob();
        auto itL = childL->z_low.begin();
        double y1 = -Signal::BigM;
        for(;itL!= childL->z_low.end(); itL++){
            if((*itL).time == tau){
                y1 = (*itL).value;
            }
        }
        double y2 = childR->compute_causation_mid_lower(tau, b);
        double y = y1<y2?y1:y2;

        return x>y?x:y;
    }

    double and_transducer::causation_opt_upper(){
        childL->causation_opt_upper();
        childR->causation_opt_upper();
        cau_up.compute_and(childL->cau_up, childR->cau_up);

        cau_up.resize(start_time,cau_up.endTime,TOP);
        if (cau_up.empty())
            cau_up.appendSample(start_time,TOP);
        return cau_up.front().value;
    }

    double and_transducer::causation_opt_lower(){
        childL->causation_opt_lower();
        childR->compute_lower_rob();
        cau_lowL.compute_and(childL->cau_low, childR->z_low);
        cau_lowL.resize(start_time, min(childL->cau_low.endTime, childR->z_low.endTime),BOTTOM);
        if(cau_lowL.empty())
            cau_lowL.appendSample(start_time, BOTTOM);

        childL->compute_lower_rob();
        childR->causation_opt_lower();
        cau_lowR.compute_and(childL->z_low, childR->cau_low);
        cau_lowR.resize(start_time, min(childL->z_low.endTime, childR->cau_low.endTime),BOTTOM);
        if(cau_lowR.empty())
            cau_lowR.appendSample(start_time, BOTTOM);

        cau_low.compute_or(cau_lowL, cau_lowR);
        cau_low.resize(start_time, cau_low.endTime, BOTTOM);
        if(cau_low.empty())
            cau_low.appendSample(start_time, BOTTOM);

        return cau_low.front().value;
    }

    
    double or_transducer::compute_lower_rob(){
        childL->compute_lower_rob();
        childR->compute_lower_rob();
        z_low.compute_or(childL->z_low,childR->z_low);
        z_low.resize(start_time,z_low.endTime,BOTTOM);
        if (z_low.empty())
            z_low.appendSample(start_time, BOTTOM);
        return z_low.front().value;
    };

    double or_transducer::compute_upper_rob(){
        childL->compute_upper_rob();
        childR->compute_upper_rob();
        z_up.compute_or(childL->z_up,childR->z_up);
        z_up.resize(start_time,min(childL->z_up.endTime,childR->z_up.endTime),TOP);
        if (z_up.empty())
            z_up.appendSample(start_time,TOP);
		
        return z_up.front().value;
    };

    double or_transducer::compute_qnmono_upper(double tau, double b){
        double x1 = childL->compute_qnmono_upper(tau, b);
        
        childR-> compute_upper_rob();
        auto itR = childR->z_up.begin();
        double x2 = Signal::BigM;
        for(;itR!=childR->z_up.end(); itR ++){
            if((*itR).time == tau){
                x2 = (*itR).value;
            }
        }

        double x = x1>x2?x1:x2;
        
        childL->compute_upper_rob();
        double y1 = Signal::BigM;
        
        auto itL = childL->z_up.begin();
        for(;itL!= childL->z_up.end(); itL ++){
            if((*itL).time == tau){
                y1 = (*itL).value;
            }
        }

        double y2 = childR->compute_qnmono_upper(tau, b);
        
        double y = y1>y2?y1:y2;
        return x<y?x:y;

        
    }

    double or_transducer::compute_qnmono_lower(double tau, double b){
        double x = childL->compute_qnmono_lower(tau, b);
        double y = childR->compute_qnmono_lower(tau, b);
        return x>y?x:y;
    }

    double or_transducer::compute_causation_mid_upper(double tau, double b){
        double x1 = childL->compute_causation_mid_upper(tau, b);

        childR-> compute_upper_rob();
        auto itR = childR->z_up.begin();
        double x2 = Signal::BigM;
        for(;itR!=childR->z_up.end(); itR ++){
            if((*itR).time == tau){
                x2 = (*itR).value;
            }
        }
        double x = x1>x2?x1:x2;

        
        childL->compute_upper_rob();
        double y1 = Signal::BigM;
        
        auto itL = childL->z_up.begin();
        for(;itL!= childL->z_up.end(); itL ++){
            if((*itL).time == tau){
                y1 = (*itL).value;
            }
        }
        double y2 = childR->compute_causation_mid_upper(tau, b);
        double y = y1>y2?y1:y2;

        return x<y?x:y;

        
    }

    double or_transducer::compute_causation_mid_lower(double tau, double b){
        double x = childL->compute_causation_mid_lower(tau, b);
        double y = childR->compute_causation_mid_lower(tau, b);
        return x>y?x:y;
    }

    double or_transducer::causation_opt_upper(){
        childL->causation_opt_upper();
        childR->compute_upper_rob();
        cau_upL.compute_or(childL->cau_up, childR->z_up);
        cau_upL.resize(start_time, min(childL->cau_up.endTime, childR->z_up.endTime), TOP);
        if(cau_upL.empty())
            cau_upL.appendSample(start_time, TOP);

        childL->compute_upper_rob();
        childR->causation_opt_upper();
        cau_upR.compute_or(childL->z_up, childR->cau_up);
        cau_upR.resize(start_time, min(childL->z_up.endTime, childR->cau_up.endTime), TOP);
        if(cau_upR.empty())
            cau_upR.appendSample(start_time, TOP);

        cau_up.compute_and(cau_upL, cau_upR);
        cau_up.resize(start_time, cau_up.endTime, TOP);
        if(cau_up.empty())
            cau_up.appendSample(start_time, TOP);

        return cau_up.front().value;
    }

    double or_transducer::causation_opt_lower(){
        childL->causation_opt_lower();
        childR->causation_opt_lower();
        cau_low.compute_or(childL->cau_low, childR->cau_low);

        cau_low.resize(start_time,cau_low.endTime, BOTTOM);
        if (cau_low.empty())
            cau_low.appendSample(start_time,BOTTOM);
        return cau_low.front().value;
    }

// IMPLIES transducer
    double implies_transducer::compute_lower_rob(){
        childL->compute_upper_rob();
        childR->compute_lower_rob();

        Signal z1;
        z1.compute_not(childL->z_up);
        z_low.compute_or(z1,childR->z_low);
        z_low.resize(start_time,z_low.endTime,BOTTOM);

        if (z_low.empty())
            z_low.appendSample(start_time, BOTTOM);
        return z_low.front().value;
    };

    double implies_transducer::compute_upper_rob(){
        childL->compute_lower_rob();
        childR->compute_upper_rob();

        Signal z1;
        z1.compute_not(childL->z_low);
        z_up.compute_or(z1,childR->z_up);
        
        z_up.resize(start_time,min(z1.endTime,childR->z_up.endTime),TOP);
        if (z_up.empty())
            z_up.appendSample(start_time,TOP);
        return z_up.front().value;
    };
    
    // NOT transducer: swap upper and lower
    double not_transducer::compute_upper_rob(){
        child->compute_lower_rob();
        if (child->z_low.empty()) {
            z_up.appendSample(start_time,TOP);
            return TOP;
        }
        z_up.compute_not(child->z_low);
        return z_up.front().value;
    }

    double not_transducer::compute_lower_rob(){
        child->compute_upper_rob();
        if (child->z_up.empty()) {
            z_low.appendSample(start_time,BOTTOM);
            return BOTTOM;
        }
        z_low.compute_not(child->z_up);
        return z_low.front().value;
    }


    double not_transducer::compute_qnmono_upper(double tau, double b){
        double dis = child->compute_qnmono_lower(tau, b);
        return -dis;
    }

    double not_transducer::compute_qnmono_lower(double tau, double b){
        double dis = child->compute_qnmono_upper(tau, b);
        return -dis;
    }
    
    double not_transducer::compute_causation_mid_upper(double tau, double b){
        double dis = child->compute_causation_mid_lower(tau, b);
        return -dis;
    }

    double not_transducer::compute_causation_mid_lower(double tau, double b){
        double dis = child->compute_causation_mid_upper(tau, b);
        return -dis;
    }

    double not_transducer::causation_opt_upper(){
        child->causation_opt_lower();
        if (child->cau_low.empty()) {
            cau_up.appendSample(start_time,TOP);
            return TOP;
        }
        cau_up.compute_not(child->cau_low);
        return cau_up.front().value;
    }

    double not_transducer::causation_opt_lower(){
        child->causation_opt_upper();
        if (child->cau_up.empty()) {
            cau_low.appendSample(start_time,BOTTOM);
            return BOTTOM;
        }
        cau_low.compute_not(child->cau_up);
        return cau_low.front().value;
    }

    // EVENTUALLY
    double ev_transducer::compute_lower_rob() {
        // lower robustness for a max operator. Partial information gives a lower bound for max, so we keep it. 

#ifdef DEBUG__
        printf( ">  ev_transducer:computer_lower_rob           IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_lower_rob();
        if (child->z_low.endTime < a) {
            z_low.appendSample(start_time, BOTTOM); 
            return BOTTOM;
        }
    
        z_low.compute_timed_eventually(child->z_low, a, b);
        double et_ =min(z_low.endTime,end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        z_low.resize(start_time,max(start_time,et), 0.);

        if (z_low.empty()) 
            z_low.appendSample(start_time, BOTTOM); 

#ifdef DEBUG__
        cout << "OUT: z_low:"<< z_low << endl;
        printf( "<  ev_transducer:computer_lower_rob           OUT.\n");
#endif
        return z_low.front().value;
    }

    double ev_transducer::compute_upper_rob() {
        // upper bound on max. Partial info can always be beaten by new samples, so can't say anything. 

#ifdef DEBUG__
        printf( ">  ev_transducer:computer_upper_rob           IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_upper_rob();

        if (child->z_up.endTime < a) {
            z_up.appendSample(start_time, TOP); 
            return TOP;
        }

        z_up.compute_timed_eventually(child->z_up, a, b);

        // Here we remove values computed with partial data 
        double et_ =min(z_up.endTime-b+a,end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        z_up.resize(start_time,et, 0.);

        if (z_up.empty()) 
            z_up.appendSample(start_time, TOP); 

#ifdef DEBUG__
        cout << "OUT: z_up:"<< z_up << endl;
        printf( "<  ev_transducer:computer_upper_rob           OUT.\n");
#endif
        return z_up.front().value;
    }


    double ev_transducer::compute_qnmono_upper(double tau, double b){
        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        double left = Signal::BigM;
        long int td_idx = 0;

        if(!double_less(b, tau + c)){
            while(td_idx <= trace_data_ptr->size()-1){
                if(double_ge((trace_data_ptr->at(td_idx))[0], tau + c)){
                    break;
                }
                td_idx ++;
            }
            while(td_idx <= trace_data_ptr->size()-1){
                double t = (trace_data_ptr->at(td_idx))[0];
                if(double_greater(t, tau + d)){
                    break;
                }
                double y = child -> compute_qnmono_upper(t, b);
                if(y < left){
                    left = y;
                }
                td_idx ++;
            }
        }
        
        compute_upper_rob();
        double right = Signal::BigM;
        auto it = z_up.begin();
        for(;it!=z_up.end();it++){
            if((*it).time == tau){
                right = (*it).value;
            }
        }

        return left>right?left:right;
    }

    double ev_transducer::compute_qnmono_lower(double tau, double b){
        double x = - Signal::BigM;

        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        long int td_idx = 0;

        if(double_less(b, tau + c))
            return x;

        while(td_idx <= trace_data_ptr->size()-1){
            if(double_ge((trace_data_ptr->at(td_idx))[0], tau + c)){
                break;
            }
            td_idx ++;
        }
        
        while(td_idx <= trace_data_ptr->size()-1){
            double t = (trace_data_ptr->at(td_idx))[0];
            //if(t > tau + d){
            if(double_greater(t, tau + d)){
                break;
            }
            double y = child -> compute_qnmono_lower(t, b);
            if(y > x){
                x = y;
            }
            td_idx ++;
        }
        return x;
    }

    double ev_transducer::compute_causation_mid_upper(double tau, double b){
        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        double left = Signal::BigM;
        long int td_idx = 0;
        while(td_idx <= trace_data_ptr->size()-1){
            if( double_ge((trace_data_ptr->at(td_idx))[0], tau + c) && double_ge((trace_data_ptr->at(td_idx))[0], tau + d - child->duration)){
                break;
            }
            td_idx ++;
        }

        while(td_idx <= trace_data_ptr->size()-1){
            double t = (trace_data_ptr->at(td_idx))[0];
            if(double_greater(t, tau+d) || double_greater(t, b)){
                break;
            }
            double y = child -> compute_causation_mid_upper(t, b);
            if(y < left){
                left = y;
            }
            td_idx ++;
        }
        
        compute_upper_rob();
        double right = Signal::BigM;
        auto it = z_up.begin();
        for(;it!=z_up.end();it++){
            if((*it).time == tau){
                right = (*it).value;
            }
        }
        return left>right?left:right;

    }

    double ev_transducer::compute_causation_mid_lower(double tau, double b){
        double x = - Signal::BigM;

        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        long int td_idx = 0;
        while(td_idx <= trace_data_ptr->size()-1){
            if(double_ge((trace_data_ptr->at(td_idx))[0], tau + c) && double_ge((trace_data_ptr->at(td_idx))[0], b - child->duration)){
                break;
            }
            td_idx ++;
        }
        
        while(td_idx <= trace_data_ptr->size()-1){
            double t = (trace_data_ptr->at(td_idx))[0];
            if(double_greater(t, tau + d) || double_greater(t, b)){
                break;
            }
            double y = child -> compute_causation_mid_lower(t, b);
            if(y > x){
                x = y;
            }
            td_idx ++;
        }
        return x;
    }

    double ev_transducer::causation_opt_upper(){
        //ZHENYA
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->causation_opt_upper();
        if (child->cau_up.endTime < a) {
            cauM.appendSample(start_time, TOP); 
        }else{
            cauM.compute_timed_globally(child->cau_up, a, b);
            double et_ =min(cauM.endTime, end_time);
            double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
            cauM.resize(start_time, max(start_time,et), 0.);
    
            if (cauM.empty()) 
                cauM.appendSample(start_time, TOP); 
        }

        child->compute_upper_rob();
        if (child->z_up.endTime < a) {
            zM.appendSample(start_time, TOP); 
        }else{
            zM.compute_timed_eventually(child->z_up, a, b);
    
            // Here we remove values computed with partial data 
            double et_ =min(zM.endTime-b+a,end_time);
            double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
            zM.resize(start_time,et, 0.);
    
            if (zM.empty()) 
                zM.appendSample(start_time, TOP);
        }
        
        cau_up.compute_or(cauM, zM);
        cau_up.resize(start_time, min(cauM.endTime, zM.endTime),TOP);
        if (cau_up.empty())
            cau_up.appendSample(start_time,TOP);
		
        return cau_up.front().value;
    }
    
    double ev_transducer::causation_opt_lower(){
        //ZHENYA
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->causation_opt_lower();
        if (child->cau_low.endTime < a) {
            cau_low.appendSample(start_time, BOTTOM); 
            return BOTTOM;
        }

        cau_low.compute_timed_eventually(child->cau_low, a, b);
        double et_ =min(cau_low.endTime, end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        cau_low.resize(start_time, max(start_time,et), 0.);

        if (cau_low.empty()) 
            cau_low.appendSample(start_time, BOTTOM); 

        return cau_low.front().value;
    }

    // ALWAYS
    double alw_transducer::compute_lower_rob() {
        // lower bound on a min operator. Partial info cannot help here. 

#ifdef DEBUG__
        printf( ">  alw_transducer:computer_lower_rob          IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_lower_rob();

        if (child->z_low.endTime < a) {
            z_low.appendSample(start_time,BOTTOM);        
            return BOTTOM;
        }
    
        z_low.compute_timed_globally(child->z_low, a, b);

        // Here we remove values computed with partial data 
        double et_ =min(z_low.endTime-b+a,end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        z_low.resize(start_time,et, 0.);
	
        if (z_low.empty()) 
            z_low.appendSample(start_time,BOTTOM);        

#ifdef DEBUG__
        printf( "OUT: z_low:");
        cout << "<  alw_transducer:computer_lower_rob           OUT."<< endl;
#endif

        return z_low.front().value;
    }

    double alw_transducer::compute_upper_rob() {
#ifdef DEBUG__
        printf( ">  alw_transducer:computer_upper_rob          IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_upper_rob();
        if (child->z_up.endTime < a) {
            z_up.appendSample(start_time, TOP); 
            return TOP;
        }

        z_up.compute_timed_globally(child->z_up, a, b);
        double et_ =min(z_up.endTime,end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        z_up.resize(start_time,max(start_time,et), 0.);

        if (z_up.empty()) 
            z_up.appendSample(start_time, TOP); 

#ifdef DEBUG__
        cout << "OUT: z_up:"<< z_up << endl;
        printf( "<  alw_transducer:computer_upper_rob          OUT.\n");
#endif
        return z_up.front().value;

    }


    double alw_transducer::compute_qnmono_upper(double tau, double b){
        double x = Signal::BigM;
        double c,d;

        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        if(double_less(b, tau + c))
            return x;

        long int td_idx = 0;

        while(td_idx <= trace_data_ptr->size()-1){
            if(double_ge((trace_data_ptr->at(td_idx))[0], tau + c)){
                break;
            }
            td_idx ++;
        }
//         while(double_less((trace_data_ptr->at(td_idx))[0], tau + c)){
//             td_idx ++;
//         }

        while(td_idx <= trace_data_ptr->size()-1){
            double t = (trace_data_ptr->at(td_idx))[0];
            if(double_greater(t, tau + d)){
                break;
            }
            double y = child -> compute_qnmono_upper(t, b);
            if(y < x){
                x = y;
            }
            td_idx ++;
        }
        return x;
    }

    double alw_transducer::compute_qnmono_lower(double tau, double b){
        
        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        double left = - Signal::BigM;
        long int td_idx = 0;
        if(!double_less(b, tau + c)){
          
            while(double_less((trace_data_ptr->at(td_idx))[0], tau + c)){
                td_idx ++;
            }
            while(td_idx <= trace_data_ptr->size()-1){
                double t = (trace_data_ptr->at(td_idx))[0];
                if(double_greater(t, tau + d)){
                    break;
                }
                double y = child -> compute_qnmono_lower(t, b);
                if(y > left){
                    left = y;
                }
                td_idx ++;
            }
        }

        compute_lower_rob();
        auto it = z_low.begin();
        double right = -Signal::BigM;
        for(;it!=z_low.end();it++){
            if((*it).time == tau){
                right = (*it).value;
            }
        }

        return left< right?left:right;
    }

    double alw_transducer::compute_causation_mid_upper(double tau, double b){
        double x = Signal::BigM;
        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        long int td_idx = 0;
        while(td_idx <= trace_data_ptr->size()-1){
            if(double_ge((trace_data_ptr->at(td_idx))[0], tau + c) && double_ge((trace_data_ptr->at(td_idx))[0], b - child->duration)){
                break;
            }
            td_idx ++;
        }

        while(td_idx <= trace_data_ptr->size()-1){
            double t = (trace_data_ptr->at(td_idx))[0];
            if(double_greater(t, tau + d) || double_greater(t, b)){
                break;
            }
            double y = child -> compute_causation_mid_upper(t, b);
            if(y < x){
                x = y;
            }
            td_idx ++;
        }
        return x;
    }

    double alw_transducer::compute_causation_mid_lower(double tau, double b){
        
        double c,d;
        if (!get_param(I->begin_str,c)) c = I->begin;
        if (!get_param(I->end_str,d)) d = I->end;

        double left = - Signal::BigM;

        long int td_idx = 0;
        
        while(td_idx <= trace_data_ptr->size()-1){
            if(double_ge((trace_data_ptr->at(td_idx))[0], tau + c) && double_ge((trace_data_ptr->at(td_idx))[0], tau + d - child->duration)){
                break;
            }
            td_idx ++;
        }

        while(td_idx <= trace_data_ptr->size()-1){
            double t = (trace_data_ptr->at(td_idx))[0];
            if(double_greater(t, tau + d) || double_greater(t, d)){
                break;
            }
            double y = child -> compute_causation_mid_lower(t, b);
            if(y > left){
                left = y;
            }
            td_idx ++;
        }

        compute_lower_rob();
        auto it = z_low.begin();
        double right = -Signal::BigM;
        for(;it!=z_low.end();it++){
            if((*it).time == tau){
                right = (*it).value;
            }
        }
        return left< right? left:right;

    }

    double alw_transducer::causation_opt_upper(){
        //ZHENYA
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->causation_opt_upper();
        if (child->cau_up.endTime < a) {
            cau_up.appendSample(start_time, TOP); 
            return TOP;
        }
        cau_up.compute_timed_globally(child->cau_up, a, b);
        double et_ =min(cau_up.endTime, end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        cau_up.resize(start_time, max(start_time,et), 0.);

        if (cau_up.empty()) 
            cau_up.appendSample(start_time, TOP); 

        return cau_up.front().value;

    }

    double alw_transducer::causation_opt_lower(){
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->causation_opt_lower();
        if (child->cau_low.endTime < a) {
            cauM.appendSample(start_time, BOTTOM); 
        }else{
            cauM.compute_timed_eventually(child->cau_low, a, b);
            double et_ =min(cauM.endTime, end_time);
            double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
            cauM.resize(start_time, max(start_time,et), 0.);
    
            if (cauM.empty()) 
                cauM.appendSample(start_time, BOTTOM); 
        }

        child->compute_lower_rob();
        if (child->z_low.endTime < a) {
            zM.appendSample(start_time, BOTTOM); 
        }else{
            zM.compute_timed_globally(child->z_low, a, b);

            double et_ =min(zM.endTime-b+a, end_time);
            double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
            zM.resize(start_time, et, 0.);
    
            if (zM.empty()) 
                zM.appendSample(start_time, BOTTOM);
        }

//         if((trace_data_ptr->at(trace_data_ptr->size()-1))[0] == 30){
//             cout << "cauM: " << cauM << endl;
//             cout << "zM: " << zM << endl;
//         }
        
        cau_low.compute_and(cauM, zM);
//         if((trace_data_ptr->at(trace_data_ptr->size()-1))[0] == 30){
//             cout << "cauM: " << cauM << endl;
//             cout << "zM: " << zM << endl;
//             cout << "cau_low: " << cau_low << endl;
//         }
        cau_low.resize(start_time, min(cauM.endTime, zM.endTime), BOTTOM);
//         if((trace_data_ptr->at(trace_data_ptr->size()-1))[0] == 30){
//             cout << "cauM: " << cauM << endl;
//             cout << "zM: " << zM << endl;
//             cout << "cau_low: " << cau_low << endl;
//         }
        if (cau_low.empty())
            cau_low.appendSample(start_time, BOTTOM);
		
        return cau_low.front().value;
        
    }


    // TODO the following is a super conservative implementation - (how) can we do better ?
    double until_transducer::compute_lower_rob() {

        //	cout << "Getting into until_transducer::compute_lower_rob" << endl;
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        if (childL->compute_lower_rob()==BOTTOM) return BOTTOM;
        if (childR->compute_lower_rob()==BOTTOM) return BOTTOM;

        z_low.compute_timed_until(childL->z_low,childR->z_low, a, b);
        double et_ =min(z_up.endTime,end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        z_low.resize(start_time,max(start_time,et),0.);

        if (z_low.empty())
            return BOTTOM;
        else
            return z_low.front().value;

    }

    double until_transducer::compute_upper_rob() {

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        if (childL->compute_upper_rob()==TOP) return TOP;
        if (childR->compute_upper_rob()==TOP) return TOP;

        z_up.compute_timed_until(childL->z_up,childR->z_up, a, b);
        double et_ =min(z_up.endTime-b,end_time);
        double et = (double)round(et_ * Signal::Freq)/Signal::Freq;
        z_up.resize(start_time,max(start_time,et),0.);

        if (z_up.empty())
            return TOP;
        else
            return z_up.front().value;
    }

}
