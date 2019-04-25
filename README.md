# PIHM input conversion tool

A simple tool to convert PIHM v2.X input files into MM-PIHM input files

## Usage

- Download the code from the [release page](https://github.com/PSUmodeling/CT-PIHM/releases)
- Use `make` command to compile the conversion tool
- Put PIHM v2.X input files into current folder
- Run the convertion tool by `./ct-pihm`, you will be prompted to enter the date and time corresponding to time *zero* in the PIHM v2.X input
- Converted input files can be found in the "output" directory

## Input check list:

The following PIHM v2.X input files are needed:

1. projectName.txt
2. \*.riv
3. \*.mesh
4. \*.att
5. \*.soil
6. \*.geol
7. \*.lc
8. \*.ibc
9. \*.para
10. \*.calib

NOTE: The meteorological forcing input format has been changed significantly from PIHM v2.X to MM-PIHM.
Thus it is recommended to generate it manually following the User's Guide of MM-PIHM.
If you prefer to use NLDAS-2 forcing data, a tool to download NLDAS-2 forcing and generate MM-PIHM meteorological forcing is provided [here](https://github.com/shiyuning/NLDAS-forcing).
