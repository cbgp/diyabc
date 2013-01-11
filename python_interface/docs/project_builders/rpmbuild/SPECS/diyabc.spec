BuildRoot:  %{_topdir}/BUILDROOT/
Summary:   User-friendly approach to Approximate Bayesian Computation
License:   GNU
Name:    diyabc
Version:   2.0
Release:   1
Group:    Biology
Requires: bash >= 3.0, python >= 2.5, PyQt4 >= 4.5, PyQwt >= 5.0

%description
DIYABC is a computer program with a graphical user interface and a fully
clickable environment. It allows population biologists to make inference based
on Approximate Bayesian Computation (ABC), in which scenarios can be customized
by the user to fit many complex situations involving any number of populations
and samples. Such scenarios involve any combination of population divergences,
admixtures and population size changes.
.
DIYABC can be used to compare competing
scenarios, estimate parameters for one or more scenarios, and compute bias and
precision measures for a given scenario and known values of parameters

%build
echo $RPM_SOURCE_DIR
cd $RPM_SOURCE_DIR;

%install
rm -fR $RPM_BUILD_ROOT;
mkdir -p $RPM_BUILD_ROOT/usr/local/bin;
mkdir -p $RPM_BUILD_ROOT/usr/local/src;
mkdir -p $RPM_BUILD_ROOT/usr/share/menu;
mkdir -p $RPM_BUILD_ROOT/usr/share/applications;

echo "#!/bin/bash
cd /usr/local/src/diyabc/
python /usr/local/src/diyabc/diyabc.py \$@" > $RPM_BUILD_ROOT/usr/local/bin/diyabc

mkdir $RPM_BUILD_ROOT/usr/local/src/diyabc/
cp -rp $RPM_SOURCE_DIR/*.py $RPM_SOURCE_DIR/clean.sh $RPM_SOURCE_DIR/analysis $RPM_SOURCE_DIR/uis $RPM_SOURCE_DIR/utils $RPM_SOURCE_DIR/summaryStatistics $RPM_SOURCE_DIR/mutationModel $RPM_SOURCE_DIR/historicalModel $RPM_SOURCE_DIR/geneticData $RPM_BUILD_ROOT/usr/local/src/diyabc/
mkdir $RPM_BUILD_ROOT/usr/local/src/diyabc/docs
cp -rp $RPM_SOURCE_DIR/docs/accueil_pictures $RPM_SOURCE_DIR/docs/icons $RPM_SOURCE_DIR/docs/executables $RPM_BUILD_ROOT/usr/local/src/diyabc/docs/
cp -r $RPM_SOURCE_DIR/docs/dev* $RPM_SOURCE_DIR/docs/doc* $RPM_BUILD_ROOT/usr/local/src/diyabc/docs/
cp $RPM_SOURCE_DIR/docs/project_builders/debian/diyabc-interface-pkg-template/usr/share/menu/diyabc $RPM_BUILD_ROOT/usr/share/menu/
cp $RPM_SOURCE_DIR/docs/project_builders/debian/diyabc-interface-pkg-template/usr/share/applications/diyabc.desktop $RPM_BUILD_ROOT/usr/share/applications/

sed -i "s/ NAMEVERSION//" $RPM_BUILD_ROOT/usr/share/menu/diyabc
sed -i "s/ICON/\/usr\/local\/src\/diyabc\/docs\/icons\/coccicon.png/" $RPM_BUILD_ROOT/usr/share/applications/diyabc.desktop
sed -i "s/ NAMEVERSION//" $RPM_BUILD_ROOT/usr/share/applications/diyabc.desktop
sed -i "s/DIYABCEXEC/\/usr\/local\/bin\/diyabc/" $RPM_BUILD_ROOT/usr/share/applications/diyabc.desktop
sed -i "s/COMMANDVERSION//" $RPM_BUILD_ROOT/usr/share/menu/diyabc



%clean
rm -rf $RPM_BUILD_ROOT/

%files
%attr(755,root,root)
/usr/

