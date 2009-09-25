function [exp_data] = make_q(exp_data, smoothy)

for day = 1:16
	for trials = 1:12
		if (day == 15 && trials > 7) || (day == 16 && trials > 6)
			break;
		end
		fprintf('Extracting Q/Qs for day %d, trial %d', day, trial);
		t_start = exp_data.trials(day, trial).start;
		t_end = exp_data.trials(day, trial).end;
		for i = 1:nbneurons	
			load(sprintf('%s/frs/%d.mat', exp_data.path, i), 'fr');
			Q(i,:) = fr(t_start:t_end);
			% we can smooth over a theta cycle to sample data
			if smoothy == 1
				fr = smooth(Q(i,:), 8);
				Qs(i,:) = fr(4:8:end-4);
			end
		end
		save('%s/frs/Q_%d_%d.mat', 'Q');
		if smoothy == 1
			save('%s/frs/Qs_%d_%d.mat', 'Qs');
		end
	end
end

exp_data.getQ = @(x,y) load(sprintf('%s/frs/Q_%d_%d.mat', exp_data.path, x, y), 'Q');
exp_data.getQs = @(x,y) load(sprintf('%s/frs/Qs_%d_%d.mat', exp_data.path, x, y), 'Qs');
