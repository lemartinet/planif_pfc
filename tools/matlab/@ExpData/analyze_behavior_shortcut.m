function analyze_behavior_shortcut(exp_data)

protocol = load(sprintf('%s/protocol.txt', exp_data.path));
events = load(sprintf('%s/events.txt', exp_data.path));

for i = 1:7
    idp = find(protocol(:, 2) == i);
    exp_data.trial(i).protocol = protocol(idp, 3);
    id = find(events(:, 2) == i);
    exp_data.trial(i).raw_choices = events(id, 4)';
    if i < 7
        exp_data.trial(i).all_choices = 0;
        exp_data.trial(i).fst_choice = 0;
    else
        m = events(id, 4)';
        m = m(ismember(m, 1:2));
        exp_data.trial(i).all_choices = m;
        if ~isempty(m)
            exp_data.trial(i).fst_choice = m(1);
        else
            exp_data.trial(i).fst_choice = 0;
        end
    end
end

end

