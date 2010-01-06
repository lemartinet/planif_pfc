# -*- coding: utf-8 -*-

import os
import sys
import glob
import numpy
from scipy import io, sparse, loadtxt, logical_and

# converti un rep de .txt en .mat
def txt2mat(dir, var, sparsify=0, prefix=""):
	print("Converting txt to matlab...")
	txtfiles = glob.glob(dir + "/%s*.txt" % prefix)
	for f in txtfiles:
		print(os.path.split(f)[1] + "... "),
		v = loadtxt(f)
		if sparsify == 1: 
			v = sparse.csc_matrix(v)
		io.savemat(os.path.splitext(f)[0] + ".mat", {var: v})
		os.remove(f)

# Extrait les fichiers d'activité
def frs(dir):
	nbneurons = loadtxt(dir + "type.txt", dtype="int", comments="%")[-1, 0];
	print("Extracting neurons...")
	os.mkdir(dir + "/frs")
	begin, end = 1, nbneurons + 1
	fin = open(dir + "/fr.txt", "r")	
	days = range(1,15) * 12 + [15] * 7 + [16] * 6
	days.sort()
	trials = range(1,13) * 14 + range(1,8) + range(1,7)
	t_idx = 0
	t_file = open(dir + "/frs/Q_%d_%d.txt" % (days[t_idx], trials[t_idx]), "w")
	var = loadtxt(dir + "vars.txt", comments="%")
	t_step = [] # t_step contient les indices de fin de chaque day/trial
	for d, t in zip(days, trials):
		t_step += [logical_and(var[:, 1] == d, var[:, 2] == t).nonzero()[0][-1]]
	for s in fin:
		rows = s.split()
		if int(rows[0]) > var[t_step[t_idx], 0]:
			t_file.close()
			t_idx += 1
			t_file = open(dir + "/frs/Q_%d_%d.txt" % (days[t_idx], trials[t_idx]), "w")
		t_file.write(" ".join(rows[1:]) + "\n")			
		if len(rows) < nbneurons + 1:
			print("line skipped")
			continue
		for i in range(begin, end):
			fout = open("%s/frs/%d.txt" % (dir, i), "a")
			fout.write(rows[i] + "\n")
			fout.close()
	t_file.close()
	fin.close()
	txt2mat (dir + "/frs/", "Q", prefix="Q_")
	txt2mat (dir + "/frs/", "fr")
	os.remove(dir + "/fr.txt")

# Extrait les fichiers de poids
def weight(dir):
	nbneurons = loadtxt(dir + "type.txt", dtype='int', comments="%")[-1, 0];
	print("Extracting weights...")
	os.mkdir(dir + "/weight/")
	fin = open(dir + "/weight.txt", "r")
	lines = []	
	nb = 0
	lines = []
	for s in fin:
		if s != "\n": 
			lines += [s]
		if len(lines) == nbneurons:
			nb += 1
			print("%d.txt ..." % nb),
			fout = open("%s/weight/%d.txt" % (dir, nb), "w")
			fout.write("".join(lines))
			fout.close()
			lines = []
	print("")
	fin.close()
	txt2mat(dir + "/weight/", "w", 1)
	os.remove(dir + "/weight.txt")
	#possibilité d'utiliser np.mat(";".join(lines)) pour construire la matrice
	#puis sparse puis on sauve la liste de toutes les matrices d'un coup

#if __name__ == "__main__":
#	# Do it !!!
#	frs(sys.argv[1], 700, int(sys.argv[2]))
#	weight(sys.argv[1], int(sys.argv[2]))

