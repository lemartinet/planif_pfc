function analyze_exploration(exp_data)

decision = load(sprintf('%s/decision.txt', exp_data.path));
i = 1;
for d = 1:15
    for t = 1:12
        if d == 15 && t > 7
            break;
        end
        dec_list = [];
        while i < size(decision, 1) && decision(i, 1) <= exp_data.trial(d, t).end
            dec_list = [dec_list, decision(i, :)'];
            i = i + 1;
        end
        exp_data.trial(d, t).decision = dec_list;
    end
end
end

