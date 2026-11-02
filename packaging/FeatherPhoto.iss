#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif

#ifndef StageDir
  #define StageDir "..\artifacts\app"
#endif

[Setup]
AppId={{80E08CF1-4A84-4BB4-9C6D-08F4C6C946B7}
AppName=Feather Photo
AppVersion={#AppVersion}
AppPublisher=Eagle Studio
DefaultDirName={autopf}\Feather Photo
DefaultGroupName=Feather Photo
UninstallDisplayIcon={app}\FeatherPhoto.exe
SetupIconFile=..\assets\FeatherPhoto.ico
OutputDir=..\artifacts
OutputBaseFilename=FeatherPhoto-Setup
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=lowest
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Feather Photo"; Filename: "{app}\FeatherPhoto.exe"
Name: "{autodesktop}\Feather Photo"; Filename: "{app}\FeatherPhoto.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\FeatherPhoto.exe"; Description: "Launch Feather Photo"; Flags: postinstall nowait skipifsilent
