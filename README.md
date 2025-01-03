# Fast hocr to csv parser

## Get the hocr file:

```SH
tesseract C:\Users\hansc\Pictures\collage_2023_04_23_13_52_30_743134.png C:\Users\hansc\Pictures\collage_2023_04_23_13_52_30_743134 -l por+eng --oem 3 -c tessedit_create_hocr=1 -c hocr_font_info=1

```

## Compile the file

```sh
g++-12 -std=c++2a -O3 -g0 hocr2csv.cpp
```

## Run the parser 

```sh
a.exe C:\Users\hansc\Pictures\collage_2023_04_23_13_52_30_743134.hocr
```