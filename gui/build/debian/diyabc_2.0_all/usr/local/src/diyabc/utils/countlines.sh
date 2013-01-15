cat *.py analysis/*.py geneticData/*.py historicalModel/*.py mutationModel/*.py summaryStatistics/*.py utils/autoPreferences.py utils/cbgpUtils.py  utils/trayIconHandler.py | grep -v "^$" | grep -vE "^ *#" | wc -l
cat *.py analysis/*.py geneticData/*.py historicalModel/*.py mutationModel/*.py summaryStatistics/*.py utils/autoPreferences.py utils/cbgpUtils.py  utils/trayIconHandler.py | wc -l
