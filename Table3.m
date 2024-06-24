clear;

model_str = {'AT', 'AFC', 'NN', 'FFR'};
model_prompt = "Input the model number: 1. AT, 2. AFC, 3. NN, 4. FFR\n";
model_id = input(model_prompt);

if model_id == 1
    spec_prompt = "Input the specification number: 1. AT1, 2. AT2, 3. AT3, 4. AT4 \n";
    spec_id = input(spec_prompt);
elseif model_id == 2
    spec_prompt = "Input the specification number: 1. AFC1, 2. AFC2 \n";
    spec_id = input(spec_prompt);
elseif model_id == 3
    spec_id = 1;
else
    spec_id = 1;
end

trace_prompt = "Input the trace ID you want to check: (1~10)\n";
trace_id = input(trace_prompt);

repeat_prompt = "Input the number you want to repeat each experiments (Default: 1)\n";
repeat = input(repeat_prompt);

plain_prompt = "Do you want to skip experiments for plain causation monitor? Y/N\n";
plain_skip = input(plain_prompt,"s");
if isempty(plain_skip) || strcmp(plain_skip, 'y') 
     plain_skip = 'Y';
end


%problem = 'NN_phi1';
problem = [model_str{model_id}, '_phi', int2str(spec_id)];

%
ids = [];
t_robM_list = [];
t_plainCau_list = [];
t_optCau_list = [];
log = {};



i = trace_id;
%for i = 1:10
    
    fileload = ['experiment/data/', problem, '_trace', int2str(i), '.mat'];
    ids = [ids; i];

    load(fileload);

    tic1 = tic;
    for j = 1:repeat
        [up_robM, low_robM] = stl_eval_mex_pw(signal_str, phi_str, trace, tau);
        log{i}{j}{1}{1} = up_robM;
        log{i}{j}{1}{2} = low_robM;
        disp([problem, ' RobM trace_', int2str(i), ' loop_', int2str(j), ' finished']);
    end
    t_robM = toc(tic1);
    t_robM_list = [t_robM_list; t_robM/repeat];
    

    if ~strcmp(plain_skip, 'Y')
        tic2 = tic;
        for j = 1:repeat
            [up_plainCau, low_plainCau] = stl_causation(signal_str, phi_str, trace, tau);
            log{i}{j}{2}{1} = up_plainCau;
            log{i}{j}{2}{2} = low_plainCau;
            disp([problem, ' t_plainCau trace_', int2str(i), ' loop_', int2str(j), ' finished']);
        end
        t_plainCau = toc(tic2);
    else
        t_plainCau = Inf;
    end
    t_plainCau_list = [t_plainCau_list; t_plainCau/repeat];
    

    tic3 = tic;
    for j = 1:repeat
        [up_optCau, low_optCau] = stl_causation_opt(signal_str, phi_str, trace, tau);
        log{i}{j}{3}{1} = up_optCau;
        log{i}{j}{3}{2} = low_optCau;
        disp([problem, ' t_optCau trace_', int2str(i), ' loop_', int2str(j), ' finished']);
    end
    t_optCau = toc(tic3);
    t_optCau_list = [t_optCau_list; t_optCau/repeat];
    

delta_rob = (t_optCau/repeat - t_robM/repeat)/(t_robM/repeat);
delta_plain = (t_optCau/repeat - t_plainCau/repeat)/(t_plainCau/repeat);
%end


fileresult = ['results/', problem, '_result', '.csv'];
%filelog = [problem, '_log', '.mat'];
%save(filelog, 'log');
result = table(ids, t_robM_list, t_plainCau_list, t_optCau_list, delta_rob, delta_plain);
writetable(result, fileresult, 'Delimiter', ';');

