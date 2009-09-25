function make_q(exp_data, smoothy)

for day = 1 : 16
    Qc = {};
    fprintf('Extracting Q for day %d', day);
    for i = 1 : exp_data.nbneurons
        fprintf(', n%d', i);
        load(sprintf('%s/frs/%d.mat', exp_data.path, i), 'fr');
        for trial = 1 : 12
            if (day == 15 && trial > 7) || (day == 16 && trial > 6)
                break;
            end
            t_start = exp_data.trial(day, trial).start;
            t_end = exp_data.trial(day, trial).end;
            % we can smooth over a theta cycle to sample data
            if smoothy == 1
                fr = smooth(fr(t_start : t_end), 8);
                Qc{trial, i} = fr(4 : 8 : end - 4);
            else
                Qc{trial, i} = fr(t_start : t_end);
            end
        end
        clear fr;
    end
%     fprintf('Saving...');
%     save(sprintf('%s/frs/Q_%d.mat', exp_data.path, day), 'Q');
%     if smoothy == 1
%         save(sprintf('%s/frs/Qs_%d.mat', exp_data.path, day), 'Qs');
%     end
    fprintf('\n\t');
    for trial = 1 : 12
        if (day == 15 && trial > 7) || (day == 16 && trial > 6)
            break;
        end
        fprintf(', trial %d', trial);
        Qtmp = {Qc{trial, :}}';
        for i = 1 : exp_data.nbneurons
           Qc{trial, i} = []; 
        end
        Q = zeros(exp_data.nbneurons, length(Qtmp{1}));
        for i = 1 : exp_data.nbneurons
           Q(i, :) = Qtmp{i};
           Qtmp{i} = [];
        end
        clear Qtmp;
        if smoothy == 1
            save(sprintf('%s/frs/Qs_%d_%d.mat', exp_data.path, day, trial), 'Q');
        else
            save(sprintf('%s/frs/Q_%d_%d.mat', exp_data.path, day, trial), 'Q');
        end
        clear Q;
    end
    clear Qc;
    fprintf('\n');
end

end
