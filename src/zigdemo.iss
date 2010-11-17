
[Setup]
AppName=ZigDemo
AppVerName=Zig Demo
AppPublisher=Scumways
DefaultDirName={pf}\ZigDemo
DefaultGroupName=ZigDemo
Compression=lzma
SolidCompression=yes

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "zigdemo.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "SDL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "SDL_mixer.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\levels"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "data\triangle.raw"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "data\invaders.png"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "data\sawtooth.raw"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "data\sine.raw"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "data\square.raw"; DestDir: "{app}\data"; Flags: ignoreversion
Source: "data\font.png"; DestDir: "{app}\data"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\ZigDemo"; Filename: "{app}\zigdemo.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,ZigDemo}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\ZigDemo"; Filename: "{app}\zigdemo.exe"; Tasks: desktopicon; WorkingDir: "{app}"

[Run]
Filename: "{app}\zigdemo.exe"; Description: "{cm:LaunchProgram,ZigDemo}"; Flags: nowait postinstall skipifsilent

