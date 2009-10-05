"""
usage: python launcher.py [-a "first last"] [-c] [-r "first last laby"]
-a: extract data for the raw outputs from "first" to "last"
-c: compile the code
-r: run the simulations from "first" to "last" using the "laby" maze (tolman_maze/tolman_maze_big)

!!! ADAPT params_base.txt BEFORE IF NEEDED !!!
"""
import getopt
import sys
import os
import time

def make():
	os.system("make -C controllers/mypuck_supervisor clean")
	os.system("make -C controllers/mypuck_supervisor -j2")
	os.system("make -C controllers/mypuck myclean")
	if os.getenv("CLUSTER") == "1":
		os.system("make -C controllers/mypuck -f Makefile_cluster -j2")
	else:
		os.system("make -C controllers/mypuck ui")
		os.system("make -C controllers/mypuck -j2")		

def compute(start, end, maze):
	wp = os.getenv("WEBOTS_HOME")
	for i in range(start, end + 1):
		print("Running simulation %d..." % i)
		os.system("rm -rf data/data_raw/%d 2> /dev/null" % i)
		os.system("mkdir data/data_raw/%d 2> /dev/null" % i)
		os.system("echo SIMULATION_ID %d > data/params.txt" % i)
		os.system("cat data/params_base.txt >> data/params.txt")
		os.system("nice -19 %s/webots --mode=fast worlds/%s.wbt" % (wp, maze))
		os.system("mv data/params.txt data/data_raw/%d" % i)
		os.system("echo %d >> data/exps" % i)
	
	# we remove project history to avoid data overwriting
	os.system("grep -vE 'defaultWorldName|projectPath' ~/.webotsrc > ~/.webotsrc")
	print("All computations finiched !")

def analyze(start, end):
	sys.path.append("tools/script")
	import post
	for i in range(start, end + 1):
		print("Processing exp %d data..." % i)
		try:
			frs("data/data_raw/%d" % i, 700, 1204)
			weight("data/data_raw/%d" % i, 1204)
		except:
			print("Problems with exp %d data" % i)

def main(argv):
	try:
		opts, args = getopt.getopt(argv, "acr:")
		if len(opts) == 0: raise getopt.GetoptError, ""
	except getopt.GetoptError:
		print(__doc__)
		sys.exit(2)
	for opt, arg in opts:
		if opt == "-c":
			make()
		if opt == "-r":
			first, last, maze = arg.split(" ")
			first, last = int(first), int(last)
			compute(first, last, maze)
		if opt == "-a":
			first, last = arg.split(" ")
			first, last = int(first), int(last)
			analyze(first, last)

if __name__ == "__main__":
	main (sys.argv[1:])

