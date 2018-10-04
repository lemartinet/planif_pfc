import sys
import os

dir = sys.argv[1]
os.mkdir(dir + "/weight/")
fin = open(dir + "/weight.txt", "r")
nb = 0
fout = open(dir + "/weight/" + str(nb) + ".txt", "w")

while 1 :
	s = fin.readline()
	if s == "" :
		break
	elif s == "\n" :
		s = fin.readline()
		fout.close ()
		nb += 1
		print "new file" + str(nb) + " " + s
		fout = open(dir + "/weight/" + str(nb) + ".txt", "w")
		continue
	fout.write(s)

fin.close()
fout.close()

