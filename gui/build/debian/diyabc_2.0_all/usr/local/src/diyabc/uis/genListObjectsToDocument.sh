echo "" > listObjectsToDocument.lst
for i in [^p]*.ui; do
    echo "ecran ${i/\.ui/}" >> listObjectsToDocument.lst
    echo "" >> listObjectsToDocument.lst
    pyuic4 $i > $i.py
    for motif in "Label" "Button" "Edit" "ListWidget" "Combo" "Check" "progressBar" "Progress"; do
        grep $motif $i.py | sed -e s/self\.//g | sed -e s/$motif.*/$motif/ | grep -v "\." | sort -u >> listObjectsToDocument.lst
    done
    echo "" >> listObjectsToDocument.lst
    rm *.py
done
cat listObjectsToDocument.lst | grep -vi "okButton" | grep -vi "exitButton" | grep -vi "clearButton" > listObjectsToDocument.lst2
mv listObjectsToDocument.lst2 listObjectsToDocument.lst

