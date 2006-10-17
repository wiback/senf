
PSTOPNG_DPI := 100

CLEAN_FILE := $(shell pwd)/.clean
clean := clean() {										    \
	touch $(CLEAN_FILE);									    \
	for file in "$$@"; do									    \
		grep "^$$file"'$$' $(CLEAN_FILE) >/dev/null || echo $$file >>$(CLEAN_FILE);	    \
	done											    \
}; clean

cmd  := run() { tput bold; echo -e "--[ $$* ]"; tput sgr0; } && run
do   := run() { tput bold; echo -e "--[ $$* ]"; tput sgr0; "$$@"; } && run
eval := run() { tput bold; echo -e "--[ $$* ]"; tput sgr0; eval "$$@"; } && run

Q := @

%.rawtex: %.stxt
	$(Q)$(clean) $@
	$(Q)$(cmd) "rest2latex $< >$@";								    \
	  rest2latex										    \
		  --language=de									    \
		  --documentclass=scrreprt							    \
		  --documentoptions="10pt,a4paper,DIV10"					    \
		  --table-style=booktabs							    \
		  --use-verbatim-when-possible							    \
		  --use-latex-footnotes								    \
		  --use-latex-toc								    \
		  --no-section-numbering							    \
		  --stylesheet=default.sty							    \
		  --exit-status=2								    \
		  $<										    \
	      > $@ || (rm -f $@; exit 1)

%.tex: %.rawtex
	$(Q)$(clean) $@
	$(Q)$(cmd) "rawfilter $< >$@";								    \
	  sed -e 's/\.\(png\|gif\|jpg\)/.pdf/g' 						    \
              -e 's/\[htbp\]/\[tbp\]/g' $< >$@ 							    \
            || (rm -f $@; exit 1)

%.html: %.stxt
	$(Q)$(clean) $@
	$(Q)$(cmd) "rest2html $< >$@";									\
	  up="`echo $< | sed -r -e 's,^[^/]*$$,,' -e 's,/[^/]+$$,/,' -e 's,[^/]+,..,g'`";		\
	  rest2html											\
		--language=de										\
		--exit-status=2										\
		--stylesheet-path="$${up}default.css"							\
		$<											\
	      > $@ || (rm -f $@; exit 1)

%.pdf: %.tex
	$(Q)(											    \
	  dir="$$(dirname $<)";									    \
	  up="echo $$dir | sed -e 's,[^/]\+,..,g";						    \
	  [ -n "$$dir" ] && dir="$$dir/";							    \
	  stem="$$(basename $*)";								    \
	  tex="$$(basename $<)";								    \
	  pdf="$$(basename $@)";								    \
	  $(clean) $${dir}texput.log;								    \
	  cd "$$dir";										    \
	  repeat=5;										    \
	  origfiles="$$(find $$stem.* ! \( -name $$pdf -o -name $$stem.log \)			    \
					    -maxdepth 0 -printf '%p-%TY%Tm%Td%TH%TM%TS\n')";	    \
	  while [ $$repeat -gt 0 ]; do								    \
	     $(cmd) "pdflatex --interaction nonstopmode $$tex";					    \
	     TEXINPUTS=.:$$up: pdflatex --interaction nonstopmode $$tex;			    \
	     pdfexit=$$?;									    \
	     auxfiles=$$((echo $$origfiles | tr ' ' '\n';					    \
			  find $$stem.* ! \( -name $$pdf -o -name $$stem.log \)			    \
					     -maxdepth 0 -printf '%p-%TY%Tm%Td%TH%TM%TS\n')	    \
			| sort | uniq -u | sed -e 's/-[0-9]*$$//' | sort -u );			    \
	     for f in $$auxfiles; do								    \
		 $(clean) $$dir$$f;								    \
	     done;										    \
	     $(clean) $*.log $@;								    \
	     [ $$pdfexit -ne 0 ] && exit $$pdfexit;						    \
	     newauxsum=`md5sum $$auxfiles | md5sum 2>/dev/null`;				    \
	     [ "$$auxsum" = "$$newauxsum" ] && break;						    \
	     auxsum="$$newauxsum";								    \
	     repeat=$$[ $$repeat - 1 ];								    \
	  done;											    \
	) || (rm -f $@; exit 1)

%.png: %.sxd
	$(Q)$(clean) $@
	$(Q)$(cmd) "(openoffice) sxdtopng $<"; \
	  openoffice -invisible "macro:///Local.Conversion.ConvertDrawToPNG($(shell realpath $<))"

%.pdf: %.sxd
	$(Q)$(clean) $@
	$(Q)$(cmd) "(openoffice) sxdtopdf $<"; \
	  openoffice -invisible "macro:///Local.Conversion.ConvertDrawToPDF($(shell realpath $<))"

%.pdf: %.sxc
	$(Q)$(clean) $@
	$(Q)$(cmd) "(openoffice) sxctopdf $<"; \
	  openoffice -invisible "macro:///Local.Conversion.ConvertCalcToPDF($(shell realpath $<))"

%.ps: %.pdf
	$(Q)$(clean) $@
	$(Q)$(cmd) "pdftops $< >$@"; \
	  pdftops -eps -nocrop -noshrink -nocenter $< - | pstops - >$@ || (rm -f $@; exit 1)

%.png: %.ps
	$(Q)$(clean) $@
	$(Q)$(cmd) "pstopng $< >$@";									\
	  bb=$$(grep -i '^%%BoundingBox:' $< | head -1 | sed -e 's/^%%BoundingBox: *//i');		\
	  bbx=$${bb% *};										\
	  bbx=$${bbx##* };										\
	  bby=$${bb##* };										\
	  pngx=$$(dc -e "$$bbx $(PSTOPNG_DPI) * 8 * 75.45 / p");					\
	  pngy=$$(dc -e "$$bby $(PSTOPNG_DPI) * 8 * 75.45 / p");					\
	  pstopnm -xborder 0 -yborder 0 -portrait -nocrop -stdout -xsize $$pngx -ysize $$pngy $<	\
	      | pnmscale -reduce 8									\
	      | pnmtopng >$@										\
	  || (rm -f $@; exit 1)

%.gif: %.png
	$(Q)$(clean) $@
	$(Q)$(cmd) "pngtogif $< >$@"
	$(Q)pngtopnm $< | ppmquant 256 | ppmtogif >$@ || ( rm -f $@; exit 1)

%.pdf: %.dot
	$(Q)$(clean) $@
	$(Q)$(eval) "dot -Tfig $< | fig2dev -L pdf -f Helvetica >$@" || ( rm -f $@; exit 1 )

%.pdf: %.neato
	$(Q)$(clean) $@
	$(Q)$(eval) "neato -Tfig $< | fig2dev -L pdf -f Helvetica >$@" || ( rm -f $@; exit 1 )

%.eps: %.dia
	$(Q)$(clean) $@
	$(Q)$(do) dia -t eps -e $@ $<

%.pdf: %.eps
	$(Q)$(clean) $@
	$(Q)$(do) epstopdf --outfile=$@ $<

%.d: %.stxt
	$(Q)$(clean) $@
	$(Q)											    \
	  $(cmd) "makedep $<";									    \
	  base="$$(dirname $<)";								    \
	  [ -n "$$base" ] && base="$${base}/";							    \
	  function scaninc {									    \
		  local includes;								    \
		  [ -r $$1 ] || return;								    \
		  includes="$$(sed -n -e 's/^.. \+include:: *//' -e T -e p $$1)";		    \
		  for include in $$includes; do							    \
			  echo $$base$$include;							    \
			  scaninc $$base$$include;						    \
		  done										    \
	  };											    \
	  includes="$$(scaninc $<)";								    \
	  figures="$$(sed -n -e "s,^.. \+figure:: *,$$base," -e T -e p $< $$includes)";		    \
												    \
	  echo "$*_Includes := $$(echo $$includes)"					     >$@;   \
	  echo "$*_Figures := $$(echo $$figures)"					    >>$@;   \
	  echo "$*_PDFs := $$(echo $$figures | sed -e 's/\.\(gif\|png\|jpg\)/.pdf/g')"	    >>$@;   \
	  echo '$@: Makefile $< $$($*_Includes)'					    >>$@;   \
	  echo '$*.html: $< $$($*_Includes)'						    >>$@;   \
	  echo '$*_html: $*.html $$($*_Figures)'					    >>$@;   \
	  echo '$*.tex: $< $$($*_Includes)'						    >>$@;   \
	  echo '$*.pdf: $*.tex $$($*_PDFs) $(TEX_STYLE)'				    >>$@

###########################################################################

clean:
	$(Q)if [ -r $(CLEAN_FILE) ]; then \
	    $(cmd) "xargs rm -f <.clean && rm -f .clean"; \
	    xargs rm -f <$(CLEAN_FILE) && rm -f $(CLEAN_FILE); \
	fi

# Local Variables:
# mode: makefile
# makefile-backslash-column: 100
# End:
