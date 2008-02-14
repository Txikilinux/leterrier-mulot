%define name abuledu-mulot
%define version 8
%define release 1
%define execname mulot.tcl
%define execpublicname mulot
%define abuledu_dest /usr/share/abuledu/applications

Summary: Mulot: Utilisation de la souris ... pour les petits
Name: %{name}
Version: %{version}
Release: %{release}
Source0: http://intranet.abuledu.org/~abuledu_andre/%{name}-%{version}.tgz
Source1:    %{name}-16.png.bz2
Source2:    %{name}-32.png.bz2
Source3:   %{name}-48.png.bz2
License: GPL 2
Group: AbulEdu
Url: http://www.abuledu.org/
BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: tcl > 8.3, tk > 8.3
Requires: tcl > 8.3, tk > 8.3, ImageMagick

%description

Motricité fine (TPS et PS de maternelle)
 Activités de manipulation fine de la souris : survol ou clic, tirer/lacher.
 Liaison entre le plan horizontal (plan de la souris) et le plan vertical (écran).
 Les images utilisées sont facilement gérées :
   o la taille recommandée maximale des images est 600x400
   o vous pouvez copier / modifier / effacer les images dans le répertoire des images!
   o en fonction de la vie de la classe, n'hésitez pas à modifier ce répertoire images, les enfants adorent

Contacts : andre.connes@wanadoo.fr et dev@abuledu.org

%prep
%setup

%build
cat > lanceur << EOF
#!/bin/sh
(
cd %{abuledu_dest}/%{name}
./%{execname}
)
EOF
chmod 755 lanceur

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{_bindir}
mkdir -p $RPM_BUILD_ROOT%{abuledu_dest}/%{name}
cp -a * $RPM_BUILD_ROOT%{abuledu_dest}/%{name}/
mkdir -p $RPM_BUILD_ROOT/var/leterrier/%{execpublicname}
cp -a * $RPM_BUILD_ROOT%{abuledu_dest}/%{name}/

install -c lanceur $RPM_BUILD_ROOT%{_bindir}/%{execpublicname}
bzcat %{SOURCE1} > %{name}-16.png
bzcat %{SOURCE2} > %{name}-32.png
bzcat %{SOURCE3} > %{name}-48.png
install -D -m 644 %{name}-16.png $RPM_BUILD_ROOT%{_miconsdir}/%{name}.png
install -D -m 644 %{name}-32.png $RPM_BUILD_ROOT%{_iconsdir}/%{name}.png
install -D -m 644 %{name}-48.png $RPM_BUILD_ROOT%{_liconsdir}/%{name}.png


(cd $RPM_BUILD_ROOT
mkdir -p ./%{_menudir}
cat > ./%{_menudir}/%{name} <<EOF
?package(%{name}):\
command="%{_bindir}/%{execpublicname}"\
title="%{execpublicname}"\
longtitle="%{name}"\
needs="x11"\
icon="%{name}.png"\
section="AbulEdu"
EOF
)
 
%post
%{update_menus}
  
%postun
%{clean_menus}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/%{execpublicname}
%{_menudir}/*
# icons
%{_iconsdir}/%{name}.png
%{_miconsdir}/%{name}.png
%{_liconsdir}/%{name}.png
%dir %{abuledu_dest}/%{name}/
%{abuledu_dest}/%{name}/*
%attr(777, root, root) /var/leterrier/%{execpublicname}

%changelog
* tue Feb 12 2008 Andre Connes <andre.connes@wanadoo.fr> 8.2
- suppression de la superposition des fenetres (fermeture de l'appli appelante)
- correction du bilan (can't read "table(...)")
- correction des messages de gestion des dossiers images
- gestion des images dans une seule fenetre
- mot de passe inutile si l'utilisateur est prof
* Mon Jan 01 2008 Andre Connes <andre.connes@wanadoo.fr> 8.1
- correction d'un bug de fermeture pour glisser_mulot et parcourir_mulot
- effacement du dossier images/ch_vases
* Thu Oct 12 2006 Andre Connes <andre.connes@wanadoo.fr> 6.10
- insertion du roumain (merci mihaela marinca)
- images du dossier sysdata en png (merci François)
* Fri Mar 17 2006 Andre Connes <andre.connes@toulouse.iufm.fr> 6.3a
- tetes neutres grises
- corrige bug pour version tcl < 8.4 (compound)
* Tue Mar 14 2006 Andre Connes <andre.connes@toulouse.iufm.fr> 6.3
- revision cosmetique (tetes, icones, suppression du superflu...)
- revision de la traduction anglaise (merci s.mcsherry)

* Sat Jan 21 2006 Andre Connes <andre.connes@toulouse.iufm.fr> 6.1
- ajout de la sauvegarde des traces-élèves au format csv
- ajout du menu Fichier > Bilans
- choix de l'aide dans l'une des langues disponibles
- suppression du menu activités (devenu inutile grace aux boutons)
- boutons internationalisés (demande T.Stauder et skol
- remplacement des boutons par les boutons de Françoiselinux-fr)
- images souriantes internationalées (cf. ci-dessus)
- corrections diverses mineures
 
* Sun Dec 25 2005 Andre Connes <andre.connes@toulouse.iufm.fr> 5.12
Le numero de version est la date YY.MM (d'apres Eric)
- ajout de l'activite double_cliquer (suggestion de ???)
- modif de l'activite glisser/deposer : (d'apres Francois Audirac)
  pour 4 vignettes (constante), le nombre de caches croit 
- suppression de l'activite chronologie
- tetes souriantes pour montrer la progression dans l'activite
  5 tetes dont 5 images minimum par theme
- on peut recommencer l'activite
- la liste des images ne presente pas de doublons
- le theme est rappele par une vignette a droite de la sortie
- plein ecran non redimensionnalisable
- remplacement du module msg.tcl : la prise en compte des accents (merci David)

* Sun Sep 28 2003 Andre Connes <andre.connes@toulouse.iufm.fr> 1.0.9-1
- New release, added clic&flyOver
* Tue Jul 15 2003 Eric Seigne <erics@hurlevent.rycks-local.com> 1.0.4-1
- New release, André added drag&drop and add new images

* Sun May 11 2003 Eric Seigne <erics@music.rycks-local.com> 1.0.0-1
- New release

* Sat Feb 15 2003 Eric Seigne <erics@music.rycks-local.com> 0.0.3-1
- First Release
