; 示例 Inno Setup 脚本

[Setup]
; 基本的安装配置
AppName=Simple Draw
AppVersion=1.0.0
DefaultDirName={pf}\SimpleDraw
DefaultGroupName=Simple Draw
OutputDir=Output
OutputBaseFilename=setup
SetupIconFile=SimpleDraw.ico
Compression=lzma2
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinese"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Files]
; 需要打包的文件
Source: "D:\workspace_year24\c++\SimpleDraw\x64\Release\SimpleDraw.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\workspace_year24\c++\SimpleDraw\x64\Release\*.dll"; DestDir: "{app}\DLL"; Flags: ignoreversion
Source: "D:\workspace_year24\c++\SimpleDraw\x64\Release\readme.txt"; DestDir: "{app}\Doc"; Flags: ignoreversion

[Icons]
; 创建桌面和开始菜单快捷方式
Name: "{group}\My Application"; Filename: "{app}\SimpleDraw.exe"
Name: "{userdesktop}\My Application"; Filename: "{app}\SimpleDraw.exe"; Tasks: desktopicon

[Tasks]
; 选择性安装任务
Name: "desktopicon"; Description: "创建桌面快捷方式"; GroupDescription: "附加任务"

[Run]
; 安装后自动运行程序
Filename: "{app}\SimpleDraw.exe"; Description: "启动 SimpleDraw"; Flags: nowait postinstall skipifsilent
