;inno setup configuration file
;mulot

[_ISTool]
EnableISX=false

[Setup]
OutputDir=..\windows

AppName=Terrier - Mulot
AppVerName=Terrier - Mulot 7.1
DefaultGroupName=Le Terrier\Mulot
DefaultDirName={pf}\Mulot
AllowRootDirectory=true
;AlwaysCreateUninstallIcon=true
DisableStartupPrompt=true
WizardImageFile=..\windows\logo-pingouin3.bmp
OutputBaseFilename=abuledu-mulot-7.1
LicenseFile=..\licence_gpl.txt
UsePreviousGroup=true
VersionInfoVersion=7.1
VersionInfoCompany=AbulEdu
VersionInfoDescription=Mulot du Terrier
VersionInfoTextVersion=Mulot du Terrier 7.1
ShowLanguageDialog=no
;SetupIconFile=C:\Program Files\Inno Setup 4\setup.ico
AppPublisher=Abuledu.org
AppPublisherURL=http://abuledu.org
AppSupportURL=http://abuledu.org
AppUpdatesURL=http://abuledu.org
AppCopyright=Le Terrier
UsePreviousAppDir=false
LanguageDetectionMethod=none
PrivilegesRequired=poweruser

[Dirs]
Name: {app}\aides
Name: {app}\images
Name: {app}\images\animaux
Name: {app}\images\paysages
Name: {app}\images\villes
Name: {app}\msgs
Name: {app}\pistes
Name: {app}\reglages; Permissions: everyone-modify
Name: {app}\sysdata
Name: {app}\sysdata\fin_sequence
Name: {app}\sysdata\fin_sequence\fr
Name: {app}\windows
Name: {app}\mulot
Name: {app}\mulot\log; Permissions: everyone-modify

[Files]
Source: ..\effacer_images.tcl; DestDir: {app}
Source: ..\effacer_dossier.tcl; DestDir: {app}
Source: ..\aider.tcl; DestDir: {app}
Source: ..\fin_sequence.tcl; DestDir: {app}
Source: ..\fonts.tcl; DestDir: {app}
Source: ..\choisir_activite.tcl; DestDir: {app}
Source: ..\glisser_mulot.tcl; DestDir: {app}
Source: ..\autoriser_menus.tcl; DestDir: {app}
Source: ..\bilan.tcl; DestDir: {app}
Source: ..\cp_bilans; DestDir: {app}
Source: ..\changelog; DestDir: {app}
Source: ..\play_mulot.tcl; DestDir: {app}
Source: ..\apropos.tcl; DestDir: {app}
Source: ..\mulot.tcl; DestDir: {app}
Source: ..\shellexec.exe; DestDir: {app}
Source: ..\eval.tcl; DestDir: {app}
Source: ..\ajouter_dossier.tcl; DestDir: {app}
Source: ..\msg.tcl; DestDir: {app}
Source: ..\mulot.conf; DestDir: {app}
Source: ..\parcourir_mulot.tcl; DestDir: {app}
Source: ..\ajouter_images.tcl; DestDir: {app}
Source: ..\licence_gpl.txt; DestDir: {app}
Source: ..\aides\aide.fr.html; DestDir: {app}\aides
Source: ..\aides\aide.en.html; DestDir: {app}\aides
Source: ..\aides\aide.it.html; DestDir: {app}\aides
Source: ..\images\animaux\chenille.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\pivert_jeune.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\agneau.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\crapaud.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\colverts.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\tetards.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\cygne.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\ours2.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\gnou_1.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\gnou_2.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\canards.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\ours.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\golden_retriver.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\papillon.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\marmotte.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\oies.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\lama.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\canard.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\loulou.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\ecureuil.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\renard_3.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\renard_4.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\renard_6.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\loulou_petit.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\bison.jpg; DestDir: {app}\images\animaux
Source: ..\images\animaux\crapauds.jpg; DestDir: {app}\images\animaux
Source: ..\images\paysages\56_pointe_du_bill.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\46_gerbes.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_pres_greze_en_vercors.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_drac.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\64-stEtienne_de_baigorry.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\es_chamoix_silhouette.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\40_dune_pilat.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_chamrousse.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\44_cote_sauvage.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_chamrousse_arrivee_telepherique.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\56_trehiguier.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_3soeurs_en-vercors.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\es_el_forato.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_vercors.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\es_kern&vignemale.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\46_causses_dolmen.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\es_campo_plano.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\46_cagnac_du_causses3.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\46_cagnac_du_causses.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\44_cote_sauvage1.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\es_baqueira.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\17_pont_ile_de_re.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\44_pen_be_ouest.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\38_pierre_percee_en_vercors.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\17_fouras.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\65_chamoix.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\44_marais.jpg; DestDir: {app}\images\paysages
Source: ..\images\paysages\44_pen-be_est.jpg; DestDir: {app}\images\paysages
Source: ..\images\villes\40_la_teste.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\38_grenoble_justice.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\81_cordes_gd_veneur.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\44_piriac.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\75_canal_stMartin.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\38_annecy.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\17_la_rochelle.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\17_stMartin_en_re.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\44_kerine.jpg; DestDir: {app}\images\villes
Source: ..\images\villes\81_cordes_halles.jpg; DestDir: {app}\images\villes
Source: ..\msgs\en.msg; DestDir: {app}\msgs
Source: ..\msgs\fr.msg; DestDir: {app}\msgs
Source: ..\msgs\oc.msg; DestDir: {app}\msgs
Source: ..\msgs\ro.msg; DestDir: {app}\msgs
Source: ..\msgs\it.msg; DestDir: {app}\msgs
Source: ..\pistes\2.inter; DestDir: {app}\pistes
Source: ..\pistes\2.union; DestDir: {app}\pistes
Source: ..\pistes\0.est; DestDir: {app}\pistes
Source: ..\pistes\0.sud; DestDir: {app}\pistes
Source: ..\pistes\3.norddroite; DestDir: {app}\pistes
Source: ..\pistes\3.nordgauche; DestDir: {app}\pistes
Source: ..\pistes\3.esthaut; DestDir: {app}\pistes
Source: ..\pistes\1.nordouest; DestDir: {app}\pistes
Source: ..\pistes\2.inclus; DestDir: {app}\pistes
Source: ..\pistes\3.ouesthaut; DestDir: {app}\pistes
Source: ..\pistes\3.suddroite; DestDir: {app}\pistes
Source: ..\pistes\3.sudgauche; DestDir: {app}\pistes
Source: ..\pistes\1.nordest; DestDir: {app}\pistes
Source: ..\pistes\2.contient; DestDir: {app}\pistes
Source: ..\pistes\1.sudouest; DestDir: {app}\pistes
Source: ..\pistes\4.piste0; DestDir: {app}\pistes
Source: ..\pistes\3.ouestbas; DestDir: {app}\pistes
Source: ..\pistes\0.nord; DestDir: {app}\pistes
Source: ..\pistes\0.ouest; DestDir: {app}\pistes
Source: ..\pistes\4.cinq; DestDir: {app}\pistes
Source: ..\pistes\4.deux; DestDir: {app}\pistes
Source: ..\pistes\1.sudest; DestDir: {app}\pistes
Source: ..\pistes\3.estbas; DestDir: {app}\pistes
Source: ..\reglages\boutons.defaut; DestDir: {app}\reglages
Source: ..\sysdata\background.png; DestDir: {app}\sysdata
Source: ..\sysdata\abuledu-mulot-16.png; DestDir: {app}\sysdata
Source: ..\sysdata\abuledu-mulot-32.png; DestDir: {app}\sysdata
Source: ..\sysdata\abuledu-mulot-48.png; DestDir: {app}\sysdata
Source: ..\sysdata\apropos.png; DestDir: {app}\sysdata
Source: ..\sysdata\apropos.xcf; DestDir: {app}\sysdata
Source: ..\sysdata\pneutre.png; DestDir: {app}\sysdata
Source: ..\sysdata\fgauche.png; DestDir: {app}\sysdata
Source: ..\sysdata\ppass.png; DestDir: {app}\sysdata
Source: ..\sysdata\pmal.png; DestDir: {app}\sysdata
Source: ..\sysdata\pbien.png; DestDir: {app}\sysdata
Source: ..\sysdata\cliquer.png; DestDir: {app}\sysdata
Source: ..\sysdata\double_cliquer.png; DestDir: {app}\sysdata
Source: ..\sysdata\glisser.png; DestDir: {app}\sysdata
Source: ..\sysdata\parcourir.png; DestDir: {app}\sysdata
Source: ..\sysdata\quitter.png; DestDir: {app}\sysdata
Source: ..\sysdata\quitter_minus.png; DestDir: {app}\sysdata
Source: ..\sysdata\fin_sequence\fr\sourire_fr.png; DestDir: {app}\sysdata\fin_sequence\fr
Source: ..\sysdata\fin_sequence\fr\sourire_ok.png; DestDir: {app}\sysdata\fin_sequence\fr
Source: ..\sysdata\fin_sequence\fr\sourire.png; DestDir: {app}\sysdata\fin_sequence\fr
Source: ..\sysdata\fin_sequence\fr\sourire_win.png; DestDir: {app}\sysdata\fin_sequence\fr
Source: ..\sysdata\survoler.png; DestDir: {app}\sysdata
Source: ..\windows\test.hta; DestDir: {app}\windows
Source: ..\windows\mulot.ico; DestDir: {app}\windows
Source: ..\windows\mulot.iss; DestDir: {app}\windows
Source: ..\windows\internat.ico; DestDir: {app}\windows
Source: ..\windows\logo-pingouin3.bmp; DestDir: {app}\windows
Source: ..\windows\auto_ts2.bat; DestDir: {app}\windows
Source: ..\windows\tst_tcl.ICO; DestDir: {app}\windows
Source: ..\windows\Gpl.ico; DestDir: {app}\windows

[Icons]
Name: {group}\Mulot; Filename: {app}\mulot.tcl; IconFilename: {app}\windows\mulot.ico; IconIndex: 0; WorkingDir: {app}
Name: {group}\Aide Mulot; Filename: {app}\aides\aide.fr.html; WorkingDir: {app}
[Messages]
SetupAppTitle=Installation
SetupWindowTitle=Installation - %1
UninstallAppTitle=Désinstallation
UninstallAppFullTitle=Désinstallation - %1
InformationTitle=Information
ConfirmTitle=Confirmation
ErrorTitle=Erreur
SetupLdrStartupMessage=Cet assistant va installer %1. Voulez-vous continuer ?
LdrCannotCreateTemp=Impossible de créer un fichier temporaire. Abandon de l'installation
LdrCannotExecTemp=Impossible d'exécuter un fichier depuis le dossier temporaire. Abandon de l'installation
LastErrorMessage=%1.%n%nErreur %2 : %3
SetupFileMissing=Le fichier %1 est absent du dossier d'installation. Veuillez corriger le problème ou vous procurer une nouvelle copie du programme.
SetupFileCorrupt=Les fichiers d'installation sont altérés. Veuillez vous procurer une nouvelle copie du programme.
SetupFileCorruptOrWrongVer=Les fichiers d'installation sont altérés ou ne sont pas compatibles avec cette version de l'assistant d'installation. Veuillez corriger le problème ou vous procurer une nouvelle copie du programme.
NotOnThisPlatform=Ce programme ne fonctionne pas sous %1.
OnlyOnThisPlatform=Ce programme ne peut fonctionner que sous %1.
WinVersionTooLowError=Ce programme requiert la version %2 ou supérieure de %1.
WinVersionTooHighError=Ce programme ne peut pas être installé sous %1 version %2 ou supérieure.
AdminPrivilegesRequired=Vous devez disposer des droits d'administration de cet ordinateur pour installer ce programme.
PowerUserPrivilegesRequired=Vous devez disposer des droits d'administration ou faire partie du groupe "Utilisateurs avec pouvoir" de cet ordinateur pour installer ce programme.
SetupAppRunningError=L'assistant d'installation a détecté que %1 est actuellement en cours d'exécution.%n%nVeuillez fermer toutes les instances de cette application puis appuyer sur OK pour continuer, ou bien appuyer sur Annuler pour abandonner l'installation.
UninstallAppRunningError=La procédure de désinstallation a détecté que %1 est actuellement en cours d'exécution.%n%nVeuillez fermer toutes les instances de cette application  puis appuyer sur OK pour continuer, ou bien appuyer sur Annuler pour abandonner la désinstallation.
ErrorCreatingDir=L'assistant d'installation n'a pas pu créer le dossier "%1"
ErrorTooManyFilesInDir=L'assistant d'installation n'a pas pu créer un fichier dans le dossier "%1" car celui-ci contient trop de fichiers
ExitSetupTitle=Quitter l'installation
ExitSetupMessage=L'installation n'est pas terminée. Si vous abandonnez maintenant, le programme ne sera pas installé.%n%nVous devrez relancer cet assistant pour finir l'installation.%n%nVoulez-vous quand même quitter l'assistant d'installation ?
AboutSetupMenuItem=&A propos...
AboutSetupTitle=A Propos de l'assistant d'installation
AboutSetupMessage=%1 version %2%n%3%n%nPage d'accueil de %1 :%n%4
AboutSetupNote=
ButtonBack=< &Précédent
ButtonNext=&Suivant >
ButtonInstall=&Installer
ButtonOK=OK
ButtonCancel=Annuler
ButtonYes=&Oui
ButtonYesToAll=Oui pour &Tout
ButtonNo=&Non
ButtonNoToAll=N&on pour Tout
ButtonFinish=&Terminer
ButtonBrowse=Pa&rcourir...
ButtonWizardBrowse=Pa&rcourir...
ButtonNewFolder=Nouveau &Dossier
SelectLanguageTitle=Langue de l'assistant d'installation
SelectLanguageLabel=Veuillez sélectionner la langue qui sera utilisée par l'assistant d'installation :
ClickNext=Appuyez sur Suivant pour continuer ou sur Annuler pour abandonner l'installation.
BeveledLabel=
BrowseDialogTitle=Parcourir les dossiers
BrowseDialogLabel=Veuillez choisir un dossier de destination, puis appuyez sur OK.
NewFolderName=Nouveau dossier
WelcomeLabel1=Bienvenue dans l'assistant d'installation de [name]
WelcomeLabel2=Cet assistant va vous guider dans l'installation de [name/ver] sur votre ordinateur.%n%nIl est recommandé de fermer toutes les applications actives avant de continuer.
WizardPassword=Mot de passe
PasswordLabel1=Cette installation est protégée par un mot de passe.
PasswordLabel3=Veuillez saisir le mot de passe (attention à la distinction entre majuscules et minuscules) puis appuyez sur Suivant pour continuer.
PasswordEditLabel=&Mot de passe :
IncorrectPassword=Le mot de passe saisi n'est pas valide. Veuillez essayer à nouveau.
WizardLicense=Accord de licence
LicenseLabel=Les informations suivantes sont importantes. Veuillez les lire avant de continuer.
LicenseLabel3=Veuillez lire le contrat de licence suivant. Vous devez en accepter tous les termes avant de continuer l'installation.
LicenseAccepted=Je comprends et j'&accepte les termes du contrat de licence
LicenseNotAccepted=Je &refuse les termes du contrat de licence
WizardInfoBefore=Information
InfoBeforeLabel=Les informations suivantes sont importantes. Veuillez les lire avant de continuer.
InfoBeforeClickLabel=Lorsque vous êtes prêt à continuer, appuyez sur Suivant.
WizardInfoAfter=Information
InfoAfterLabel=Les informations suivantes sont importantes. Veuillez les lire avant de continuer.
InfoAfterClickLabel=Lorsque vous êtes prêt à continuer, appuyez sur Suivant.
WizardUserInfo=Informations sur l'Utilisateur
UserInfoDesc=Veuillez saisir les informations qui vous concernent.
UserInfoName=&Nom d'utilisateur :
UserInfoOrg=&Organisation :
UserInfoSerial=Numéro de &série :
UserInfoNameRequired=Vous devez au moins saisir un nom.
WizardSelectDir=Dossier de destination
SelectDirDesc=Où [name] doit-il être installé ?
SelectDirLabel3=L'assistant va installer [name] dans le dossier suivant.
SelectDirBrowseLabel=Pour continuer, appuyez sur Suivant. Si vous souhaitez choisir un dossier différent, appuyez sur Parcourir.
DiskSpaceMBLabel=Le programme requiert au moins [mb] Mo d'espace disque disponible.
ToUNCPathname=L'assistant d'installation ne supporte pas les chemins réseau. Si vous souhaitez effectuer cette installation sur un réseau, vous devez d'abord connecter un lecteur réseau.
InvalidPath=Vous devez saisir un chemin complet avec sa lettre de lecteur ; par exemple :%n%nC:\APP%n%nou un chemin réseau de la forme :%n%n\\serveur\partage
InvalidDrive=L'unité ou l'emplacement réseau que vous avez sélectionné n'existe pas ou n'est pas accessible. Veuillez choisir une autre destination.
DiskSpaceWarningTitle=Espace disponible insuffisant
DiskSpaceWarning=L'assistant a besoin d'au moins %1 Ko d'espace disponible pour effectuer l'installation, mais l'unité que vous avez sélectionné ne dispose que de %2 Ko d'espace disponible.%n%nSouhaitez-vous continuer malgré tout ?
DirNameTooLong=Le nom ou le chemin du dossier est trop long.
InvalidDirName=Le nom du dossier est invalide.
BadDirName32=Le nom du dossier ne doit contenir aucun des caractères suivants :%n%n%1
DirExistsTitle=Dossier existant
DirExists=Le dossier :%n%n%1%n%nexiste déjà. Souhaitez-vous installer dans ce dossier malgré tout ?
DirDoesntExistTitle=Le dossier n'existe pas
DirDoesntExist=Le dossier %n%n%1%n%nn'existe pas. Souhaitez-vous que ce dossier soit créé ?
WizardSelectComponents=Composants à installer
SelectComponentsDesc=Quels composants de l'application souhaitez-vous installer ?
SelectComponentsLabel2=Sélectionnez les composants que vous désirez installer ; décochez les composants que vous ne désirez pas installer. Appuyez ensuite sur Suivant pour continuer l'installation.
FullInstallation=Installation complète
CompactInstallation=Installation compacte
CustomInstallation=Installation personnalisée
NoUninstallWarningTitle=Composants existants
NoUninstallWarning=L'assistant d'installation a détecté que les composants suivants sont déjà installés sur votre système :%n%n%1%n%nDésélectionner ces composants ne les désinstallera pas pour autant.%n%nVoulez-vous continuer malgré tout ?
ComponentSize1=%1 Ko
ComponentSize2=%1 Mo
ComponentsDiskSpaceMBLabel=Les composants sélectionnés nécessitent au moins [mb] Mo d'espace disponible.
WizardSelectTasks=Tâches supplémentaires
SelectTasksDesc=Quelles sont les tâches supplémentaires qui doivent être effectuées ?
SelectTasksLabel2=Sélectionnez les tâches supplémentaires que l'assistant d'installation doit effectuer pendant l'installation de [name], puis appuyez sur Suivant.
WizardSelectProgramGroup=Sélection du dossier du menu Démarrer
SelectStartMenuFolderDesc=Où l'assistant d'installation doit-il placer les raccourcis du programme ?
SelectStartMenuFolderLabel3=L'assistant va créer les raccourcis du programme dans le dossier du menu Démarrer indiqué ci-dessous.
SelectStartMenuFolderBrowseLabel=Appuyez sur Suivant pour continuer. Appuyez sur Parcourir si vous souhaitez sélectionner un autre dossier du menu Démarrer.
NoIconsCheck=&Ne pas créer d'icône
MustEnterGroupName=Vous devez saisir un nom de dossier du menu Démarrer.
GroupNameTooLong=Le nom ou le chemin du dossier est trop long.
InvalidGroupName=Le nom du dossier n'est pas valide.
BadGroupName=Le nom du dossier ne doit contenir aucun des caractères suivants :%n%n%1
NoProgramGroupCheck2=Ne pas créer de &dossier dans le menu Démarrer
WizardReady=Prêt à installer
ReadyLabel1=L'assistant dispose à présent de toutes les informations pour installer [name] sur votre ordinateur.
ReadyLabel2a=Appuyez sur Installer pour procéder à l'installation ou sur Précédent pour revoir ou modifier une option d'installation.
ReadyLabel2b=Appuyez sur Installer pour procéder à l'installation.
ReadyMemoUserInfo=Informations sur l'utilisateur :
ReadyMemoDir=Dossier de destination :
ReadyMemoType=Type d'installation :
ReadyMemoComponents=Composants sélectionnés :
ReadyMemoGroup=Dossier du menu Démarrer :
ReadyMemoTasks=Tâches supplémentaires :
WizardPreparing=Préparation de l'installation
PreparingDesc=L'assistant d'installation prépare l'installation de [name] sur votre ordinateur.
PreviousInstallNotCompleted=L'installation ou la suppression d'un programme précédent n'est pas totalement achevée. Veuillez redémarrer votre ordinateur pour achever cette installation ou suppression.%n%nUne fois votre ordinateur redémarré, veuillez relancer cet assistant pour reprendre l'installation de [name].
CannotContinue=L'assistant ne peut pas continuer. Veuillez appuyer sur Annuler pour abandonner l'installation.
WizardInstalling=Installation en cours
InstallingLabel=Veuillez patienter pendant que l'assistant installe [name] sur votre ordinateur.
FinishedHeadingLabel=Fin de l'installation de [name]
FinishedLabelNoIcons=L'assistant a terminé l'installation de [name] sur votre ordinateur.
FinishedLabel=L'assistant a terminé l'installation de [name] sur votre ordinateur. L'application peut être lancée à l'aide des icônes créées sur le Bureau par l'installation.
ClickFinish=Veuillez appuyer sur Terminer pour quitter l'assistant d'installation.
FinishedRestartLabel=L'assistant doit redémarrer votre ordinateur pour terminer l'installation de [name].%n%nVoulez-vous redémarrer maintenant ?
FinishedRestartMessage=L'assistant doit redémarrer votre ordinateur pour terminer l'installation de [name].%n%nVoulez-vous redémarrer maintenant ?
ShowReadmeCheck=Oui, je souhaite lire le fichier LISEZMOI
YesRadio=&Oui, redémarrer mon ordinateur maintenant
NoRadio=&Non, je préfère redémarrer mon ordinateur plus tard
RunEntryExec=Exécuter %1
RunEntryShellExec=Voir %1
ChangeDiskTitle=L'assistant a besoin du disque suivant
SelectDiskLabel2=Veuillez insérer le disque %1 et appuyer sur OK.%n%nSi les fichiers de ce disque se trouvent à un emplacement différent de celui indiqué ci-dessous, veuillez saisir le chemin correspondant ou appuyez sur Parcourir.
PathLabel=&Chemin :
FileNotInDir2=Le fichier "%1" ne peut pas être trouvé dans "%2". Veuillez insérer le bon disque ou sélectionner un autre dossier.
SelectDirectoryLabel=Veuillez indiquer l'emplacement du disque suivant.
SetupAborted=L'installation n'est pas terminée.%n%nVeuillez corriger le problème et relancer l'installation.
EntryAbortRetryIgnore=Appuyez sur Réessayer pour essayer à nouveau, Ignorer pour continuer malgré tout, ou Abandonner pour annuler l'installation.
StatusCreateDirs=Création des dossiers...
StatusExtractFiles=Extraction des fichiers...
StatusCreateIcons=Création des raccourcis...
StatusCreateIniEntries=Création des entrées du fichier INI...
StatusCreateRegistryEntries=Création des entrées de registre...
StatusRegisterFiles=Enregistrement des fichiers...
StatusSavingUninstall=Sauvegarde des informations de désinstallation...
StatusRunProgram=Finalisation de l'installation...
StatusRollback=Annulation des modifications...
ErrorInternal2=Erreur interne : %1
ErrorFunctionFailedNoCode=%1 a échoué
ErrorFunctionFailed=%1 a échoué ; code %2
ErrorFunctionFailedWithMessage=%1 a échoué ; code %2.%n%3
ErrorExecutingProgram=Impossible d'exécuter le fichier :%n%1
ErrorRegOpenKey=Erreur lors de l'ouverture de la clé de registre :%n%1\%2
ErrorRegCreateKey=Erreur lors de la création de la clé de registre :%n%1\%2
ErrorRegWriteKey=Erreur lors de l'écriture de la clé de registre :%n%1\%2
ErrorIniEntry=Erreur d'écriture d'une entrée dans le fichier INI "%1".
FileAbortRetryIgnore=Appuyez sur Réessayer pour essayer à nouveau, Ignorer pour passer ce fichier (déconseillé), ou Abandonner pour annuler l'installation.
FileAbortRetryIgnore2=Appuyez sur Réessayer pour essayer à nouveau, Ignorer pour continuer malgré tout (déconseillé), ou Abandonner pour annuler l'installation.
SourceIsCorrupted=Le fichier source est altéré
SourceDoesntExist=Le fichier source "%1" n'existe pas
ExistingFileReadOnly=Le fichier existant est protégé en lecture seule.%n%nAppuyez sur Réessayer pour enlever la protection et essayer à nouveau, Ignorer pour passer ce fichier, ou Abandonner pour annuler l'installation.
ErrorReadingExistingDest=Une erreur s'est produite lors de la tentative de lecture du fichier existant :
FileExists=Le fichier existe déjà.%n%nSouhaitez-vous que l'installation le remplace ?
ExistingFileNewer=Le fichier existant est plus récent que celui que l'assistant essaie d'installer. Il est recommandé de conserver le fichier existant.%n%nSouhaitez-vous conserver le fichier existant ?
ErrorChangingAttr=Une erreur est survenue en essayant de modifier les attributs du fichier existant :
ErrorCreatingTemp=Une erreur est survenue en essayant de créer un fichier dans le dossier de destination :
ErrorReadingSource=Une erreur est survenue lors de la lecture du fichier source :
ErrorCopying=Une erreur est survenue lors de la copie d'un fichier :
ErrorReplacingExistingFile=Une erreur est survenue lors du remplacement d'un fichier existant :
ErrorRestartReplace=Le marquage d'un fichier pour remplacement au redémarrage de l'ordinateur a échoué :
ErrorRenamingTemp=Une erreur est survenue en essayant de renommer un fichier dans le dossier de destination :
ErrorRegisterServer=Impossible d'enregistrer la bibliothèque DLL/OCX : %1
ErrorRegisterServerMissingExport=La fonction exportée DllRegisterServer n'a pas été trouvée
ErrorRegisterTypeLib=Impossible d'enregistrer la bibliothèque de type : %1
ErrorOpeningReadme=Une erreur est survenue à l'ouverture du fichier LISEZMOI.
ErrorRestartingComputer=L'installation n'a pas pu redémarrer l'ordinateur. Merci de bien vouloir le faire vous-même.
UninstallNotFound=Le fichier "%1" n'existe pas. Impossible de désinstaller.
UninstallOpenError=Le fichier "%1" n'a pas pu être ouvert. Impossible de désinstaller
UninstallUnsupportedVer=Le format du fichier journal de désinstallation "%1" n'est pas reconnu par cette version de la procédure de désinstallation. Impossible de désinstaller
UninstallUnknownEntry=Une entrée inconnue (%1) a été rencontrée dans le fichier journal de désinstallation
ConfirmUninstall=Voulez-vous vraiment désinstaller complètement %1 ainsi que tous ses composants ?
OnlyAdminCanUninstall=Ce programme ne peut être désinstallé que par un utilisateur disposant des droits d'administration.
UninstallStatusLabel=Veuillez patienter pendant que %1 est retiré de votre ordinateur.
UninstalledAll=%1 a été correctement désinstallé de cet ordinateur.
UninstalledMost=La désinstallation de %1 est terminée.%n%nCertains éléments n'ont pas pu être supprimés automatiquement. Vous pouvez les supprimer manuellement.
UninstalledAndNeedsRestart=Vous devez redémarrer l'ordinateur pour terminer la désinstallation de %1.%n%nVoulez-vous redémarrer maintenant ?
UninstallDataCorrupted=Le ficher "%1" est altéré. Impossible de désinstaller
ConfirmDeleteSharedFileTitle=Supprimer les fichiers partagés ?
ConfirmDeleteSharedFile2=Le système indique que le fichier partagé suivant n'est plus utilisé par aucun programme. Souhaitez-vous que la désinstallation supprime ce fichier partagé ?%n%nSi des programmes utilisent encore ce fichier et qu'il est supprimé, ces programmes ne pourront plus fonctionner correctement. Si vous n'êtes pas sûr, choisissez Non. Laisser ce fichier dans votre système ne posera pas de problème.
SharedFileNameLabel=Nom du fichier :
SharedFileLocationLabel=Emplacement :
WizardUninstalling=Etat de la désinstallation
StatusUninstalling=Désinstallation de %1...
