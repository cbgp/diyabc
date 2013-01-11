SOURCEDIR=../../
PACKAGESRCDIR=/home/julien/rpmbuild/SOURCES
PACKAGEDIR=/home/julien/rpmbuild

rm -rf $PACKAGEDIR
cp -r ./rpmbuild/ $PACKAGEDIR
cp -rp $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $PACKAGESRCDIR/
mkdir $PACKAGESRCDIR/docs
cp -rp $SOURCEDIR/docs/accueil_pictures $SOURCEDIR/docs/icons $SOURCEDIR/docs/executables $SOURCEDIR/docs/project_builders/ $PACKAGESRCDIR/docs/
cp -r $SOURCEDIR/docs/dev* $SOURCEDIR/docs/doc* $PACKAGESRCDIR/docs/

rpmbuild --target i686  -v -bb $PACKAGEDIR/SPECS/diyabc.spec
cp $PACKAGEDIR/RPMS/*/* ./
