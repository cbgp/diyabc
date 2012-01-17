# cleaning destination directory
rm -rf Notice_DIYABC_principal2

# writing l2h config
echo "\$DVIPSOPT = '';" > ~/.latex2html-init
echo "1;" >>  ~/.latex2html-init

# modifiying header of tex file to help l2h
echo "\documentclass [a4paper]{article}" > Notice_DIYABC_principal2.tex
echo "\usepackage{html}
%\begin{htmlonly}
%\newenvironment{tabularx}[2]{\begin{tabular}{#2}}{\end{tabular}}
%\end{htmlonly}" >> Notice_DIYABC_principal2.tex
# avoiding mistakes on section numerotation
cat Notice_DIYABC_principal.tex | grep -v sectionv | grep -v renewcommand | grep -v documentclass >> Notice_DIYABC_principal2.tex

# processing latex2html convertion
latex2html Notice_DIYABC_principal2.tex -split 0 -show_section_numbers

rm -rf ../python_interface/docs/documentation/
mv Notice_DIYABC_principal2 ../python_interface/docs/documentation
