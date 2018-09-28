function [l] = build_distrib(v)
    repv = ones(1, length(v));
    v=v/sum(v);
    for i=1:length(v)
     repv(i)=sum(v(1:i));
    end
    l=[];
    for j=1:10000
        l=[l, min(find(repv-rand()>0))];
    end
end