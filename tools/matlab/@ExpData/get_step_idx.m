function [idx] = get_step_idx(exp_data, days, trials)

idx = [];
v1 = sort([exp_data.trial(days, trials).start] / exp_data.step_size);
v2 = sort([exp_data.trial(days, trials).end] / exp_data.step_size);
for i = 1:length(v1)
    idx = [idx, v1(i):v2(i)];
end

end