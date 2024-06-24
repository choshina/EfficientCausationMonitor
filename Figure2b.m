clear;

problem = 'AFC_phi1';


i = 9;
    
fileload = ['experiment/data/', problem, '_trace', int2str(i), '.mat'];
load(fileload);

if startsWith(problem, 'AFC')
    ori_trace = trace;
    str_ = strrep(phi_str,' ','');
    phi_str = strrep(str_, 'abs(AF[t]-AFref[t])', 'u[t]');
    signal_str = 'u';
    u_tr = abs(trace(2,:) - trace(3,:));
    trace = [trace(1,:); u_tr];
end


[up_robM, low_robM] = stl_eval_mex_pw(signal_str, phi_str, trace, tau);
%[up_plainCau, low_plainCau] = stl_causation(signal_str, phi_str, trace, tau);
[up_optCau, low_optCau] = stl_causation_opt(signal_str, phi_str, trace, tau);

signal_list = split(signal_str, ',');
num_sig = numel(signal_list);

t = trace(1, :);

%% ======== signals  ======
%begin plotting
close 
f = figure(1);
subplot(3,1,1);

if startsWith(problem, 'AFC')
    plot(t, ori_trace(2, :)', t, ori_trace(3, :)', 'm', 'LineWidth', 2);
    set(gca, 'LineWidth', 2, 'FontSize',18)
    legend({'AF','AFref'});
    grid on;
    xlim([0 50]);
    xticks(0:5:50);
elseif startsWith(problem, 'AT_phi3')
    yyaxis right
    plot(t, trace(2,:)',  'm', 'LineWidth', 2);
    set(gca, 'LineWidth', 2, 'FontSize',18)
    
    yyaxis left
    plot(t, trace(3,:)', 'g', 'LineWidth', 2);
    legend({'speed','RPM'});
    grid on;
    xlim([0 30]);
    xticks(0:5:30);

else
    plot(t, trace(2,:)', 'm', 'LineWidth', 2);
    set(gca, 'LineWidth', 2, 'FontSize',18)
    legend({'speed'});
    grid on;
    xlim([0 30]);
    xticks(0:5:30);
end

g = title(phi_str);
set(g,'Interpreter','None')
subplot(3,1,2);
hold on;

%==================== robust ================

stairs(t(2:end), up_robM(2:end)',  'LineWidth', 2);
stairs(t(2:end), low_robM(2:end)',  'LineWidth', 2);

set(gca, 'LineWidth', 2, 'FontSize',18)
set(gcf,'position',[10,10,800,500])

if startsWith(problem, 'AFC')
    xlim([0 50]);
    xticks(0:5:50);

    ylim([-15 15]);
    yticks(-15:10:15);
  
else
    xlim([0 30]);
    xticks(0:5:30);
end


legend({'Upper robustness','Lower robustness'});
grid on;

% %=========== plain causation =================
% subplot(4,1,3);
% hold on;
% 
% stairs(t(2:end), up_plainCau(2:end)',  'LineWidth', 2);
% stairs(t(2:end), low_plainCau(2:end)',  'LineWidth', 2);
% 
% set(gca, 'LineWidth', 2, 'FontSize',18)
% set(gcf,'position',[10,10,800,500])
% if startsWith(problem, 'AFC')
%     xlim([0 50]);
%     xticks(0:5:50);
%     
%     ylim([-0.5 0.5]);
%     yticks(-0.5:0.5:0.5);
% else
%     xlim([0 30]);
%     xticks(0:5:30);
% end
% 
% legend({'violation causation (plain)','satisfaction causation (plain)'});
% grid on;

%=========== efficient causation =================
subplot(3,1,3);
hold on;

stairs(t(2:end), up_optCau(2:end)',  'LineWidth', 2);
stairs(t(2:end), low_optCau(2:end)',  'LineWidth', 2);

set(gca, 'LineWidth', 2, 'FontSize',18)
set(gcf,'position',[10,10,800,500])
if startsWith(problem, 'AFC')
    xlim([0 50]);
    xticks(0:5:50);

    ylim([-15 15]);
    yticks(-15:10:15);
    
else
    xlim([0 30]);
    xticks(0:5:30);
end

legend({'violation causation','satisfaction causation'});
grid on;

%%
%save2pdf('RobustOnlinePlot.pdf')   
exportgraphics(f, strcat('results/', 'Figure2b.png'),'Resolution',300)
