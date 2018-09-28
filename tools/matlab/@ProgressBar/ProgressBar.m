classdef ProgressBar < handle
% example
% 
%     pbar = ProgressBar();
%     for i = 1:length(N)
%         pbar.update(i/length(N));
%         % your computation here ...
%     end

    properties
        progress_bar_position
        relapsed_time
    end
    
    methods
        function [p] = ProgressBar()
            p.progress_bar_position = 0;
            p.relapsed_time = 0.01;
            tic;
        end
        
        function update(p, status)
            p.progress_bar_position=status;
            p.display();
        end
        
        function display(p)
            clc;
            disp('|=================================================|');
            progress_string='|';
            for counter = 1:floor(p.progress_bar_position*100/2),
               progress_string=[progress_string,'#'];
            end
            p.relapsed_time=max(toc,0.01);
            disp(progress_string);
            completed=floor(p.progress_bar_position*100);

            if completed<100
                disp(['|================= ',num2str(completed),'% completed =================|']);
            else
                disp('|================ 100 % completed ================|');
            end
            minutes=floor((p.relapsed_time/p.progress_bar_position-p.relapsed_time)/ 60);
            seconds=rem((floor(p.relapsed_time/p.progress_bar_position-p.relapsed_time)),60);
            disp(' ');
            if (seconds>9)
                disp(['            Estimated remaining time: ', num2str(minutes),':',num2str(seconds)]);
            elseif (p.progress_bar_position<1)
                disp(['            Estimated remaining time: ', num2str(minutes),':0',num2str(seconds)]);
            end
            minutes=floor(p.relapsed_time/ 60);
            seconds=rem((floor(p.relapsed_time)),60);
            if (seconds>9)
                disp(['            Elapsed time:             ', num2str(minutes),':',num2str(seconds)]);
            else
                disp(['            Elapsed time:             ', num2str(minutes),':0',num2str(seconds)]);
            end
        end
    end
end