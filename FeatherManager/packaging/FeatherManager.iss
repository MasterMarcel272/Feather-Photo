#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif

#ifndef StageDir
  #define StageDir "..\artifacts\manager-app"
#endif

[Setup]
AppId={{6E581A82-F112-4C43-A8D0-E2E6C6F61A60}
AppName=Feather Manager
AppVersion={#AppVersion}
AppPublisher=Eagle Studio
DefaultDirName={autopf}\Feather Manager
DefaultGroupName=Feather Manager
UninstallDisplayIcon={app}\FeatherManager.exe
SetupIconFile=..\assets\FeatherManager.ico
OutputDir=..\artifacts\manager
OutputBaseFilename=FeatherManager-Setup
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
Name: "{group}\Feather Manager"; Filename: "{app}\FeatherManager.exe"
Name: "{autodesktop}\Feather Manager"; Filename: "{app}\FeatherManager.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\FeatherManager.exe"; Description: "Launch Feather Manager"; Flags: postinstall nowait skipifsilent
