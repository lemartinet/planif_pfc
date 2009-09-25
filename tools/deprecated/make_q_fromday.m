function [] = make_q_fromday(exp_data, smoothy)

for day = 13 : 16
    fprintf('Extracting Q/Qs for day %d', day);
    Qc = load(sprintf('%s/frs/Q_%d.mat', exp_data.path, day));
    Qc = Qc.Q;
    if smoothy == 1
        Qcs = load(sprintf('%s/frs/Qs_%d.mat', exp_data.path, day));
        Qcs = Qcs.Qs;
    end
    for trial = 1 : 12
        fprintf(', trial %d', trial);
        if (day == 15 && trial > 7) || (day == 16 && trial > 6)
            break;
        end
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
        save(sprintf('%s/frs/Q_%d_%d.mat', exp_data.path, day, trial), 'Q');
        clear Q;
        if smoothy == 1
            Qs = cell2mat({Qcs{trial, :}}');
            save(sprintf('%s/frs/Qs_%d_%d.mat', exp_data.path, day, trial), 'Qs');
            clear Qs;
        end
    end
    clear Qc Qcs;
    fprintf('\n');
end

