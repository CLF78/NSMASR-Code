Compiling the Loader:
* Use CodeWrite or pyasmiih to compile NSMASRLoader.S (preferably the former, the second is untested)
* Open an hex editor and create a new file
* Paste the compiled code in the new file, removing anything before "48000150"

Applying the XML Patches:
* Download WIT and install it
* Open a command line window
* Type ´wit dolpatch DOLFILE xml=XMLFILE -s "FOLDERWITHLOADER"´ where DOLFILE is the dol you want to patch, XMLFILE is xmlpatches.xml and FOLDERWITHLOADER is a folder containing Loader.bin
