
mkdir Misc
cd Misc

ECHO > FILESINLOOP.txt
FOR /F "USEBACKQ DELIMS=|" %%G IN (`dir /B /A:-D "..\*.h"`) DO (
	ECHO %%G >> FILESINLOOP.txt
	MKLINK "%%G" "..\%%G"
)

mkdir BaseObjects
cd BaseObjects

ECHO > FILESINLOOP.txt
FOR /F "USEBACKQ DELIMS=|" %%G IN (`dir /B /A:-D "..\*.h"`) DO (
	ECHO %%G >> FILESINLOOP.txt
	MKLINK "%%G" "..\%%G"
)

cd ..
