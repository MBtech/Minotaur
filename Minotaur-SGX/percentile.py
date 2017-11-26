import numpy
import sys

a = numpy.loadtxt(sys.argv[2])

with open(sys.argv[1]+"_50", "a") as myfile:
    myfile.write(str(numpy.percentile(a,50))+"\n")

with open(sys.argv[1]+"_90", "a") as myfile:
    myfile.write(str(numpy.percentile(a,90))+"\n")

with open(sys.argv[1]+"_99", "a") as myfile:
    myfile.write(str(numpy.percentile(a,99))+"\n")
