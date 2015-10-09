GOTO L_EndComment
This bat-file is used to generate parser code from language description.
08/01/2011
:L_EndComment
REM Generating parser from grammar description...
Coco.exe HLSL.atg -o "../impl/"
getkey.exe
