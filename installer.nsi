Unicode true

!include "MUI.nsh"

!define MUI_ABORTWARNING # This will warn the user if they exit from the installer.

!insertmacro MUI_PAGE_WELCOME # Welcome to the installer page.
!insertmacro MUI_PAGE_DIRECTORY # In which folder install page.
!insertmacro MUI_PAGE_INSTFILES # Installing page.
!insertmacro MUI_PAGE_FINISH # Finished installation page.

!insertmacro MUI_LANGUAGE "English"

Name exec-helper
OutFile "installer.exe"
InstallDir '$PROGRAMFILES64\exec-helper'

# Default section
Section
    SetOutPath $INSTDIR

    # Files to install
    File /a bin\exec-helper.exe
    File /a bin\eh.exe
    File /a bin\lua51.dll
    File /a share\exec-helper\LICENSE
    File /a share\exec-helper\plugins\*

    # define uninstaller name
    WriteUninstaller "$INSTDIR\uninstaller.exe"

    # Register app path subkey
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\exec-helper.exe" '' '$INSTDIR\exec-helper.exe'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\exec-helper.exe" 'Path' '$INSTDIR'

    # Register application subkey
    WriteRegStr HKCR "Applications\exec-helper.exe\shell\open\command" '' '$INSTDIR\exec-helper.exe'

    # Add to PATH
    EnVar::AddValue "PATH" "$INSTDIR"
      Pop $0
    DetailPrint "EnVar::AddValue returned=|$0|"
SectionEnd

# Uninstall instructions
Section "Uninstall"
    # Always delete uninstaller first
    Delete '$INSTDIR\uninstaller.exe'
   
    # now delete installed files
    Delete '$INSTDIR\exec-helper.exe'
    Delete '$INSTDIR\eh.exe'
    Delete '$INSTDIR\LICENSE'

    # Delete the install folder
    RMDir '$INSTDIR'

    # Unregister app path subkey
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\exec-helper.exe"

    # Unregister application subkey
    DeleteRegKey HKCR "Applications\exec-helper.exe"

    # Delete from PATH
    EnVar::DeleteValue "PATH" "$INSTDIR"
    Pop $0
    DetailPrint "EnVar::DeleteValue returned=|$0|"
SectionEnd
