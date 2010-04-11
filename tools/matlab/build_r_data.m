function [data, index] = build_r_data(group, thresh)

if iscell(group)
    val = [group{:}];
else
	val = group(:)';
end
data = val(rand(size(val)) >= thresh);
index = ones(size(data));

end