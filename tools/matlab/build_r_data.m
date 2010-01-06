function [data] = build_r_data(group, thresh)

if iscell(group)
    val = [group{:}];
else
	val = group(:)';
end
data = val(rand(size(val)) >= thresh);

end