import os
import Gnuplot

print "répertoire à traiter"
repertoire = raw_input()

os.chdir(repertoire)

ls = os.listdir(".")

for fichier in ls :
	print "Displaying mean error in " + fichier
	g = Gnuplot.Gnuplot(persist=1)
	g("set title \'" + fichier + "\'")
	g("set xrange [0:100000]")
	g.plot("\'" + fichier + "/meanError\' w l title \'erreur locale moyenne\'")
	
