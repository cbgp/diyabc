BuildRoot:  %{_topdir}/BUILDROOT/
Summary:   User-friendly approach to Approximate Bayesian Computation
License:   GNU
Name:    diyabc-gui
Version:   X
Release:   1
Group:    Biology
Requires: bash >= 3.0, python >= 2.5, PyQt4 >= 4.5, diyabc-doc >= X

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
mkdir -p $RPM_BUILD_ROOT/usr/bin;
mkdir -p $RPM_BUILD_ROOT/usr/local/src/diyabc;
mkdir -p $RPM_BUILD_ROOT/usr/share/menu;
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/diyabc;
mkdir -p $RPM_BUILD_ROOT/usr/share/images/diyabc;
mkdir -p $RPM_BUILD_ROOT/usr/share/applications;

echo "#!/bin/bash
cd /usr/local/src/diyabc/
python /usr/local/src/diyabc/diyabc.py \$@" > $RPM_BUILD_ROOT/usr/bin/diyabc-gui
chmod +x $RPM_BUILD_ROOT/usr/bin/diyabc-gui

cp -rp $RPM_SOURCE_DIR/src/* $RPM_BUILD_ROOT/usr/local/src/diyabc/
cp -rp $RPM_SOURCE_DIR/images/* $RPM_BUILD_ROOT/usr/share/images/diyabc/
cp -rp $RPM_SOURCE_DIR/icons/* $RPM_BUILD_ROOT/usr/share/icons/diyabc/
cp $RPM_SOURCE_DIR/menu/diyabc $RPM_BUILD_ROOT/usr/share/menu/
cp $RPM_SOURCE_DIR/applications/diyabc.desktop $RPM_BUILD_ROOT/usr/share/applications/

sed -i "s/ NAMEVERSION//" $RPM_BUILD_ROOT/usr/share/menu/diyabc
sed -i "s/ICON/\/usr\/local\/src\/diyabc\/docs\/icons\/coccicon.png/" $RPM_BUILD_ROOT/usr/share/applications/diyabc.desktop
sed -i "s/ NAMEVERSION//" $RPM_BUILD_ROOT/usr/share/applications/diyabc.desktop
sed -i "s/DIYABCEXEC/\/usr\/bin\/diyabc-gui/" $RPM_BUILD_ROOT/usr/share/applications/diyabc.desktop
sed -i "s/COMMANDVERSION//" $RPM_BUILD_ROOT/usr/share/menu/diyabc



%clean
rm -rf $RPM_BUILD_ROOT/

%files
%attr(755,root,root)
/usr/

