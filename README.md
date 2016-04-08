# ct-pihm
A simple tool to convert old version of pihm to MM-PIHM

Instruction:
- Put old version of input files into current folder;
- Enter "make" to make ct-pihm;
- Run the convertion tool by "./ct-pihm", enter date and time for starting time point;
- Find input files for new MM-PIHM in output folder.

Input check list:
 0) projectName.txt 
 1) *.riv
 2) *.mesh
 3) *.att
 4) *.soil
 5) *.geol
 6) *.lc
 7) *.ibc
 8) *.para
 9) *.calib

* The meteorological forcing input changes a lot in the new version of MM-PIHM, so it is recommended by regenerating it againfollowing the guide of MM-PIHM. And it can also be download from NLDAS-2 dataset.
