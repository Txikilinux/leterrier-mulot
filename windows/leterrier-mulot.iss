; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
; Exemple AppId={{62974C2F-0013-4262-AF5E-7F46C992672E}
AppId={{7188AA57-B55B-4321-88B8-1797A5DA49F2}
AppName=Le Terrier d'AbulEdu - Mulot
AppVersion=LAVERSION
AppPublisher=RyX�o SARL
AppPublisherURL=http://www.abuledu.org/leterrier/
AppSupportURL=http://www.abuledu.org/leterrier/
AppUpdatesURL=http://www.abuledu.org/leterrier/
DefaultDirName={pf}\AbulEdu\Mulot
DefaultGroupName=Le Terrier d'AbulEdu
LicenseFile=../gpl-3.0.txt
OutputDir=.
OutputBaseFilename=leterrier-mulot-LAVERSION-setup
Compression=lzma
SolidCompression=yes
SignTool=ryxeo /d $qPackage d'installation$q $f
SignedUninstaller=True
SignedUninstallerDir=.

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl" 

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\release\leterrier-mulot.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\data\*"; DestDir: "{app}\data"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "..\conf\*"; DestDir: "{app}\conf"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "../lang/*.qm"; DestDir: "{app}\lang"; Flags: ignoreversion
Source: "C:/code/quazip.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:/code/exiv2/bin/libexiv2-12.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:/QtSDK/mingw/bin/libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/mingw/bin/mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/mingw/bin/libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtScript4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtNetwork4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtGUI4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtSQL4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtSVG4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtXml4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtWebKit4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins\imageformats/qgif4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins\imageformats/qico4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins\imageformats/qjpeg4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins\imageformats/qmng4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins\imageformats/qsvg4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins\imageformats/qtiff4.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/QtMultimedia4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins/phonon_backend\phonon_ds94.dll"; DestDir: "{app}\phonon_backend"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:/QtSDK/Desktop/Qt/4.8.1/mingw/bin/phonon4.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Mulot"; Filename: "{app}\leterrier-mulot.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\Mulot"; Filename: "{app}\leterrier-mulot.exe"; Tasks: desktopicon; WorkingDir: "{app}"

[Run]
Filename: "{app}\leterrier-mulot.exe"; Description: "{cm:LaunchProgram,Mulot}"; Flags: nowait postinstall skipifsilent
