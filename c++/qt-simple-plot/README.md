# qt-simple-plot
A simple Qt 5 plotting application

## Compile and launch
>git clone https://github.com/marcoscarpetta/qt-simple-plot  
>cd qt-simple-plot  
>qmake  
>make  
>./qt-simple-plot /path/to/input-file  

## Input file format
x1 y1 x2 y2 x3 y3 .....  
x1 y1 x2 y2 x3 y3 .....  
x1 y1 x2 y2 x3 y3 .....  
.....  
  
or  
x1 y1  
x1 y1  
...  
x1 y1  
x2 y2 (x2 < x1)  
x2 y2  
...  
x2 y2  
x3 y3 (x3 < x2)  
...  
