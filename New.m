clear;

spec_id = 1;
problem = ['CAR_phi', int2str(spec_id)];
repeat = 5;
i = 1;

%
ids = [];
t_robM_list = [];
t_plainCau_list = [];
t_optCau_list = [];
log = {};

fileload = ['experiment/data/', problem, '_trace', int2str(i), '.mat'];
ids = [ids; i];

load(fileload);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Robust monitor
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic1 = tic;
for j = 1:repeat
    [up_robM, low_robM] = stl_eval_mex_pw(signal_str, phi_str, trace, tau);
    log{i}{j}{1}{1} = up_robM;
    log{i}{j}{1}{2} = low_robM;
    disp([problem, ' t_robM trace_', int2str(i), 'loop_', int2str(j), 'finished']);
end
t_robM = toc(tic1);
t_robM_list = [t_robM_list; t_robM/repeat];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Causation monitor
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
tic3 = tic;
for j = 1:repeat
    [up_optCau, low_optCau] = stl_causation_opt(signal_str, phi_str, trace, tau);
    log{i}{j}{3}{1} = up_optCau;
    log{i}{j}{3}{2} = low_optCau;
    disp([problem, ' t_optCau trace_', int2str(i), 'loop_', int2str(j), 'finished']);
end
t_optCau = toc(tic3);
t_optCau_list = [t_optCau_list; t_optCau/repeat];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



fileresult = ['results/', problem, '_result', '.csv'];
result = table(ids, t_robM_list,  t_optCau_list);
writetable(result, fileresult, 'Delimiter', ';');


