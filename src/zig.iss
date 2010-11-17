
[Setup]
AppName=Zig
AppVerName=Zig
AppPublisher=Scumways
DefaultDirName={pf}\Zig
DefaultGroupName=Zig
Compression=lzma
SolidCompression=yes

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "zig.exe"; DestDir: "{app}"; Flags: ignoreversion
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
Name: "{group}\Zig"; Filename: "{app}\zig.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,Zig}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\Zig"; Filename: "{app}\zig.exe"; Tasks: desktopicon; WorkingDir: "{app}"

[Run]
Filename: "{app}\zig.exe"; Description: "{cm:LaunchProgram,Zig}"; Flags: nowait postinstall skipifsilent

