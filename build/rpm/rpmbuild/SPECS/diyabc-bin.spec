BuildRoot:  %{_topdir}/BUILDROOT/
Summary:   User-friendly approach to Approximate Bayesian Computation
License:   GNU
Name:    diyabc-bin
Version:   X
Release:   1
Group:    Biology
#Requires:
BuildArch: noarch
URL: http://www1.montpellier.inra.fr/CBGP/diyabc/
Distribution: whatever
Packager: julien@aiur.fr

%description
This package provides the computation executable of DIYABC.
.
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
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man1/;
cp $RPM_SOURCE_DIR/diyabc.1.gz $RPM_BUILD_ROOT/usr/share/man/man1/;

cp -rp $RPM_SOURCE_DIR/usr/bin/diyabc $RPM_BUILD_ROOT/usr/bin/;

%clean
rm -rf $RPM_BUILD_ROOT/

%files
%attr(755,root,root)
/usr/

