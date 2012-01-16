rm -rf Notice_DIYABC_principal2
echo "\$DVIPSOPT = '';" > ~/.latex2html-init
echo "1;" >>  ~/.latex2html-init
cat Notice_DIYABC_principal.tex | grep -v sectionv | grep -v renewcommand | sed -e s/report/article/ > Notice_DIYABC_principal2.tex
latex2html Notice_DIYABC_principal2.tex -split 0 -show_section_numbers
