fIn = open("ZoneExpert", "r")

print "noeud a afficher"
numNoeud = input()
print "nombre d'iteration"
nbIteration = input()

fOut = open("Affichage" + "_" + str(numNoeud) + "_" + str(nbIteration), "w")

fOut.write("unset label\nset xrange [0:5000]\nset yrange [0:5000]\n")
dico = {}
lastS = ""
while 1 :
    s = fIn.readline()
    if lastS == s :
        continue
    else :
        lastS = s
    colonnes = s.split()
    if len(colonnes) == 0 :
        break
    if float(colonnes[4]) > 0.1 :
        continue
    if dico.has_key(colonnes[3]) :
        dico[colonnes[3]].append((colonnes[1], colonnes[2]))
    else :
        dico[colonnes[3]] = []

i = 1
for cle in dico :
    if int(cle) == numNoeud or numNoeud == -1 :
        if nbIteration == -1 :
            l = dico[cle]
        else :
            l = dico[cle][-nbIteration:]
        for element in l :
            fOut.write("set label " + str(i) + " \"" + cle + "\" at " + element[0] + "," + element[1] + "\n")
            i = i+1

fOut.write("plot 0\n")
fIn.close()
fOut.close()

