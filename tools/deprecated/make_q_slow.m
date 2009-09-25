function [exp_data] = make_q_slow(exp_data, smoothy)

for day = 1 : 16
	for trial = 1 : 12
		if (day == 15 && trial > 7) || (day == 16 && trial > 6)
			break;
		end
		fprintf('Extracting Q/Qs for day %d, trial %d: ', day, trial);
		t_start = exp_data.trial(day, trial).start;
		t_end = exp_data.trial(day, trial).end;
        Q = zeros(exp_data.nbneurons, length(t_start : t_end));
		for i = 1 : exp_data.nbneurons
            fprintf('n%d... ', i);
			load(sprintf('%s/frs/%d.mat', exp_data.path, i), 'fr');
			Q(i,:) = fr(t_start : t_end);
			% we can smooth over a theta cycle to sample data
			if smoothy == 1
				fr = smooth(Q(i,:), 8);
				Qs(i,:) = fr(4 : 8 : end - 4);
			end
		end
		save(sprintf('%s/frs/Q_%d_%d.mat', exp_data.path, day, trial), 'Q');
        clear Q;
		if smoothy == 1
			save(sprintf('%s/frs/Qs_%d_%d.mat', exp_data.path, day, trial), 'Qs');
            clear Qs;
        end
        fprintf('\n');
	end
end

exp_data.getQ = @(x,y) load(sprintf('%s/frs/Q_%d_%d.mat', exp_data.path, x, y), 'Q');
exp_data.getQs = @(x,y) load(sprintf('%s/frs/Qs_%d_%d.mat', exp_data.path, x, y), 'Qs');
