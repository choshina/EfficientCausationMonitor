clear;

problem = 'AT_phi1';

%
ids = [];
t_robM_list = [];
t_plainCau_list = [];
t_optCau_list = [];
log = {};

repeat = 2;

for i = 1:10
    
    fileload = ['data/', problem, '_trace', int2str(i), '.mat'];
    ids = [ids; i];

    load(fileload);

    tic1 = tic;
    for j = 1:repeat
        [up_robM, low_robM] = stl_eval_mex_pw(signal_str, phi_str, trace, tau);
        log{i}{j}{1}{1} = up_robM;
        log{i}{j}{1}{2} = low_robM;
        disp([problem, ' t_robM trace_', int2str(i), 'loop_', int2str(j), 'finished']);
    end
    t_robM = toc(tic1);
    t_robM_list = [t_robM_list; t_robM/5.0];
    

    tic2 = tic;
    for j = 1:repeat
        [up_plainCau, low_plainCau] = stl_causation(signal_str, phi_str, trace, tau);
        log{i}{j}{2}{1} = up_plainCau;
        log{i}{j}{2}{2} = low_plainCau;
        disp([problem, ' t_plainCau trace_', int2str(i), 'loop_', int2str(j), 'finished']);
    end
    t_plainCau = toc(tic2);
    t_plainCau_list = [t_plainCau_list; t_plainCau/5.0];
    

    tic3 = tic;
    for j = 1:repeat
        [up_optCau, low_optCau] = stl_causation_opt(signal_str, phi_str, trace, tau);
        log{i}{j}{3}{1} = up_optCau;
        log{i}{j}{3}{2} = low_optCau;
        disp([problem, ' t_optCau trace_', int2str(i), 'loop_', int2str(j), 'finished']);
    end
    t_optCau = toc(tic3);
    t_optCau_list = [t_optCau_list; t_optCau/5.0];
    
end


fileresult = [problem, '_result', '.csv'];
filelog = [problem, '_log', '.mat'];
save(filelog, 'log');
result = table(ids, t_robM_list, t_plainCau_list, t_optCau_list);
writetable(result, fileresult,'Delimiter',';');

