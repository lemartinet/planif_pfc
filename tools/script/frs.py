# arg1 : path de l'exp
# arg2 : (> 700) nb neuron = last index + 1
# il faudrait lire cette valeur dans le fichier type.txt !!!
# faire une suite au script qui convertit chaque .txt en .mat

import os
import sys

part1 = 700
part2 = int(sys.argv[2])

dir = sys.argv[1]

os.mkdir(dir + "/frs")
fin = open(dir + "/fr.txt", "r")
fout = []
for i in range (1, part1+1):
	fout.append(open(dir + "/frs/" + str(i) + ".txt", "w"))

while 1 :
	s = fin.readline()
	if s == "" :
		break
	rows = s.split()
	if len(rows) < part2 + 1:
		print "line skipped"
		continue
	for i in range (1,part1 + 1):
		fout[i-1].write(rows[i] + "\n")

fin.close()
for file in fout:
	file.close()

fin = open(dir + "/fr.txt", "r")
fout = []
for i in range (part1+1, part2+1):
	fout.append(open(dir + "/frs/" + str(i) + ".txt", "w"))

while 1 :
	s = fin.readline()
	if s == "" :
		break
	rows = s.split()
	if len(rows) < part2 + 1:
		print "line skipped"
		continue
	for i in range (part1 + 1, part2 + 1):
		fout[i-(part1 + 1)].write(rows[i] + "\n")

fin.close()
for file in fout:
	file.close()


