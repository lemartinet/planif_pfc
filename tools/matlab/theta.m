function [bin, nb] = theta( folder_in, type )

t = load( sprintf('%s/type.txt', folder_in) );
idx = t( find(t(:,2) == type), 1 );
for i = 1:size(idx,1)
	fr = load( sprintf('%s/frs/%d.mat', folder_in, idx(i)) ); 
	for j=1:8
		bin(i,j) = sum( fr(j:8:size(fr, 1)) );
		nb(i,j) = size( j:8:size(fr, 1), 2 );
	end
end

%load theta_10.mat bin nb;
%for i=1:size(bin, 1)
%binn(i,:) = bin(i,:) ./ sum(bin(i,:));
%end
%clf;hold on;
%for i=1:size(bin, 1)/2
%plot (binn(i,:));
%end

%load theta_0.mat bin nb;
%for i=1:size(bin, 1)
%binn(i,:) = bin(i,:) ./ sum(bin(i,:));
%end
%for i=1:size(bin, 1)/2
%plot (binn(i,:));
%end

%function [m] = theta( folder_in, type )
%t = load( sprintf('%s/type.txt', folder_in) );
%idx = t( find(t(:,2) == type), 1 );
%hold on
%for i = 1:size(idx,1)
%load( sprintf('%s/frs/%d.mat', folder_in, idx(i)), 'fr' ); 
%m(i) = max(fr);
%%plot( bin(i,:) ./ nb(i,:) );
%end

endfunction

