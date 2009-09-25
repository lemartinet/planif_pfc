import sys
import os

dir = sys.argv[1]
os.mkdir(dir + "/weight/")
fin = open(dir + "/weight.txt", "r")
nb = 0
fout = open(dir + "/weight/" + str(nb) + ".txt", "w")
ligne = 0

while 1 :
	s = fin.readline()
	if s == "" :
		break
	elif s == "\n" :
		s = fin.readline()
		fout.close ()
		nb += 1
		print( str(nb) + ".txt ... " ),
		fout = open(dir + "/weight/" + str(nb) + ".txt", "w")
		ligne = 0
		continue
	fout.write(s)
	ligne += 1

fin.close()
fout.close()

if ligne == 0 :
	os.remove(dir + "/weight/" + str(nb) + ".txt")
print( "\n" )
