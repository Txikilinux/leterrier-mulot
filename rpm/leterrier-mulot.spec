Name: 		leterrier-mulot
Version: 	1.1
Release: 	%mkrel 1
License: 	GPLv2+
Summary: 	A mouse learning software
Group:		Education
#Source: 	devrait etre http://quelquepart/%name-%version.tar.xz
Source: 	%name-%version.tar.xz
URL:		http://www.abuledu.org/leterrier/mulot
BuildRequires:	qt4-devel
BuildRequires:	ccache
#erics libqt4-devel gcc-c++ qtwebkit-devel ccache libexiv2-devel libexiv2_11 lib64exiv2-devel lib64exiv2_11
Suggests:	mulot-data

%description
Mulot est un logiciel comportant plusieurs activités permettant la prise en main de la souris. Au delà de l'aspect technique c'est un support motivant la production langagière et plus particulièrement la description d'image et l'anticipation.
Il contient un éditeur d'exercices. 

%files
%{_bindir}/%{name}
%{_libdir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop

%prep
%setup -q -n %{name}-%{version}

%build
qmake
%make

%install
mkdir -p %{buildroot}%{_bindir}
install -D -m755 debian/%{name}.wrapper %{buildroot}%{_bindir}/%{name}
#correction du dossier lib en x86_64
sed -i -e s+/usr/lib/+%{_libdir}/+  %{buildroot}%{_bindir}/%{name}

install -D -m755 %{name}  %{buildroot}%{_libdir}/%{name}/%{name}

mkdir -p %{buildroot}%{_datadir}/applications/
install  -m755 debian/%{name}.desktop %{buildroot}%{_datadir}/applications/
#correction chemin de l'icone
sed -i -e s+/%{name}/+/%{name}/data/icones/+  %{buildroot}%{_datadir}/applications/%{name}.desktop

mkdir -p %{buildroot}%{_datadir}/%{name}/lang
install -m644 lang/*.qm %{buildroot}%{_datadir}/%{name}/lang/
mv data/ %{buildroot}%{_datadir}/%{name}/

