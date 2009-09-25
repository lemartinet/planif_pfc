function [] = txt2mat( folder_in )

t = load( sprintf('%s/type.txt', folder_in) );
printf( 'Converting neurone ' ); fflush(stdout); 
for i = 1:size(t,1)
printf( '%d... ', t(i,1) ); fflush(stdout); 
fr = load( sprintf('%s/frs/%d.txt', folder_in, t(i,1)) ); 
save('-mat', sprintf('%s/frs/%d.mat', folder_in, t(i,1)), 'fr');
end

printf( 'Converting weight matrix ' ); fflush(stdout); 
list = readdir( sprintf('%s/weight', folder_in) );
nbw = size(list, 1) - 2;
for i=0:nbw-1
printf( '%.2f%%... ', i/nbw ); fflush(stdout); 
w = load( sprintf('%s/weight/%d.txt', folder_in, i) );
w = sparse(w);
save('-mat', sprintf('%s/weight/%d.mat', folder_in, i), 'w');
end

endfunction

