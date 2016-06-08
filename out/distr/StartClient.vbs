Set WshShell = CreateObject("WScript.Shell")
WshShell.Run chr(34) & "c:\My\Tankist\out\distr\Tankist_Client.bat"  & Chr(34), 0
Set WshShell = Nothing