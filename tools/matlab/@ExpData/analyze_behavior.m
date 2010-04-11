function analyze_behavior(exp_data)

protocol = load(sprintf('%s/protocol.txt', exp_data.path));
events = load(sprintf('%s/events.txt', exp_data.path));
for i = 1:15
	for j = 1:12
        if i == 15 && j > 7
            break;
        end
        idp = find(protocol(:, 2) == i & protocol(:, 3) == j);
        exp_data.trial(i, j).protocol = protocol(idp, 4);
        id = find(events(:, 2) == i & events(:, 3) == j);
        exp_data.trial(i, j).raw_choices = events(id, 5)';
        m = events(id, 5)';
        m = m(ismember(m, 1:3));
        if i > 1 && protocol(idp, 4) ~= 4 && length(m) > 0 && m(1) == 1
            % we remove the first decision because it's a forced P1
            m = m(2:end);
        end
        exp_data.trial(i, j).all_choices = m;
        if ~isempty(m)
            exp_data.trial(i, j).fst_choice = m(1);
        else
            exp_data.trial(i, j).fst_choice = [];
        end
	end
end
end

