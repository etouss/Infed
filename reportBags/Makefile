
SRC= abstract.tex \
     motivation.tex \
     logics.tex \
     rworks.tex \
     fragmentsimple.tex \
		 fragmentcard.tex \
		 fragmentorder.tex \
		 fragmentspen.tex \
		 fragmentimpl.tex \
     conclusion.tex \
		 acknow.tex \
		 appendix.tex \
     dp-mset.bib

all: main.pdf

main.bbl: main.tex dp-mset.bib
	pdflatex -file-line-error main.tex
	bibtex main.aux

main.pdf: $(SRC) main.bbl
	pdflatex -file-line-error main.tex

clean:
	rm *.syn* *.aux *.log *~ *.bbl *.bst *.out
