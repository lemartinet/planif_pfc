%%% tracé de la figure des potentiels neurones V dans les données de karim
%%% on utilise une interpolation ts les 0.01 bins, à partir d'un histog 
%%% calculé tous les 0.3 bins

load myset
v=[4,8,14,23,24,28,33,35,36,37]
v=v([2,3,4,6,7,8, 9])
load DataLouisXIX181014

figure, hold on, for i=v
[n,x]=hist(PlacePref(i,:),0:0.3:4);
plot(0:0.01:4,interp1(x,n, 0:0.01:4, 'pchip'))
end