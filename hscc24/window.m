clear;

addpath('/home/zhenya/EfficientCausationMonitor/Online/bin/');
problem = 'AT_phi2';

%
ids = [];
rates = [];
t_robM_list = [];
t_plainCau_list = [];
t_optCau_list = [];
log = {};

repeat = 5;
id = 5;

for i = 1:6
    
    fileload = ['data/', problem, '_trace', int2str(id), '.mat'];
    rates = [rates; i];

    load(fileload);

    if i == 1
        phi_str = 'alw_[0,29](not(speed[t]>70) or ev_[0,1](speed[t] > 80))';
    elseif i == 2
        phi_str = 'alw_[0,29](not(speed[t]>70) or ev_[0,3](speed[t] > 80))';
    elseif i == 3
        phi_str = 'alw_[0,29](not(speed[t]>70) or ev_[0,5](speed[t] > 80))';
    elseif i == 4
        phi_str = 'alw_[0,29](not(speed[t]>70) or ev_[0,7](speed[t] > 80))';
    elseif i == 5
        phi_str = 'alw_[0,29](not(speed[t]>70) or ev_[0,9](speed[t] > 80))';
    elseif i == 6
        phi_str = 'alw_[0,29](not(speed[t]>70) or ev_[0,11](speed[t] > 80))';
    end
%     tic1 = tic;
%     for j = 1:repeat
%         [up_robM, low_robM] = stl_eval_mex_pw(signal_str, phi_str, trace, tau);
%         log{i}{j}{1}{1} = up_robM;
%         log{i}{j}{1}{2} = low_robM;
%         disp([problem, ' t_robM trace_', int2str(i), 'loop_', int2str(j), 'finished']);
%     end
%     t_robM = toc(tic1);
%     t_robM_list = [t_robM_list; t_robM/repeat];
%     
% 
%     tic2 = tic;
%     for j = 1:repeat
%         [up_plainCau, low_plainCau] = stl_causation(signal_str, phi_str, trace, tau);
%         log{i}{j}{2}{1} = up_plainCau;
%         log{i}{j}{2}{2} = low_plainCau;
%         disp([problem, ' t_plainCau trace_', int2str(i), 'loop_', int2str(j), 'finished']);
%     end
%     t_plainCau = toc(tic2);
%     t_plainCau_list = [t_plainCau_list; t_plainCau/repeat];
    

    tic3 = tic;
    for j = 1:repeat
        [up_optCau, low_optCau] = stl_causation_opt(signal_str, phi_str, trace, tau);
        log{i}{j}{3}{1} = up_optCau;
        log{i}{j}{3}{2} = low_optCau;
        disp([problem, ' t_optCau trace_', int2str(i), 'loop_', int2str(j), 'finished']);
    end
    t_optCau = toc(tic3);
    t_optCau_list = [t_optCau_list; t_optCau/repeat];
    
end


fileresult = [problem, '_window_result', '.csv'];
filelog = [problem, '_log', '.mat'];
save(filelog, 'log');
result = table(rates, t_optCau_list);
writetable(result, fileresult,'Delimiter',';');