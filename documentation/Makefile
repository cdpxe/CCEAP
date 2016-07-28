PANDOC=pandoc
INPUTFILE=README.md
OUTPUTFILE=docs.pdf

all : $(INPUTFILE)
	$(PANDOC) -V colorlinks --reference-links $(INPUTFILE) -o $(OUTPUTFILE)

e :
	gedit $(INPUTFILE)

v :
	evince $(OUTPUTFILE)

