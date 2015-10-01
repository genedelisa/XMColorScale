typedef struct appdata {
     Boolean gotColorResfile;
     Boolean gotResfile;
     Boolean debug;
     Boolean forceNoncolor;
     char *rgbFile;
     char *personalRgbFile;
} AppData;

extern AppData appdata;
