#include <stdio.h>

// High Intensity Colors
void colorHighIntensityBlack() { printf("\033[0;90m"); }
void colorHighIntensityRed() { printf("\033[0;91m"); }
void colorHighIntensityGreen() { printf("\033[0;92m"); }
void colorHighIntensityYellow() { printf("\033[0;93m"); }
void colorHighIntensityBlue() { printf("\033[0;94m"); }
void colorHighIntensityMagenta() { printf("\033[0;95m"); }
void colorHighIntensityCyan() { printf("\033[0;96m"); }
void colorHighIntensityWhite() { printf("\033[0;97m"); }

// Bold High Intensity Colors
void colorBoldHighIntensityBlack() { printf("\033[1;90m"); }
void colorBoldHighIntensityRed() { printf("\033[1;91m"); }
void colorBoldHighIntensityGreen() { printf("\033[1;92m"); }
void colorBoldHighIntensityYellow() { printf("\033[1;93m"); }
void colorBoldHighIntensityBlue() { printf("\033[1;94m"); }
void colorBoldHighIntensityMagenta() { printf("\033[1;95m"); }
void colorBoldHighIntensityCyan() { printf("\033[1;96m"); }
void colorBoldHighIntensityWhite() { printf("\033[1;97m"); }

// Standard Colors
void colorBlack() { printf("\033[0;30m"); }
void colorRed() { printf("\033[0;31m"); }
void colorGreen() { printf("\033[0;32m"); }
void colorYellow() { printf("\033[0;33m"); }
void colorBlue() { printf("\033[0;34m"); }
void colorMagenta() { printf("\033[0;35m"); }
void colorCyan() { printf("\033[0;36m"); }
void colorWhite() { printf("\033[0;37m"); }

// Bright Colors
void colorBrightBlack() { printf("\033[1;30m"); }
void colorBrightRed() { printf("\033[1;31m"); }
void colorBrightGreen() { printf("\033[1;32m"); }
void colorBrightYellow() { printf("\033[1;33m"); }
void colorBrightBlue() { printf("\033[1;34m"); }
void colorBrightMagenta() { printf("\033[1;35m"); }
void colorBrightCyan() { printf("\033[1;36m"); }
void colorBrightWhite() { printf("\033[1;37m"); }

// Italic High Intensity Colors
void colorItalicHighIntensityBlack() { printf("\033[3;90m"); }
void colorItalicHighIntensityRed() { printf("\033[3;91m"); }
void colorItalicHighIntensityGreen() { printf("\033[3;92m"); }
void colorItalicHighIntensityYellow() { printf("\033[3;93m"); }
void colorItalicHighIntensityBlue() { printf("\033[3;94m"); }
void colorItalicHighIntensityMagenta() { printf("\033[3;95m"); }
void colorItalicHighIntensityCyan() { printf("\033[3;96m"); }
void colorItalicHighIntensityWhite() { printf("\033[3;97m"); }

// Italic Bold High Intensity Colors
void colorItalicBoldHighIntensityBlack() { printf("\033[3;1;90m"); }
void colorItalicBoldHighIntensityRed() { printf("\033[3;1;91m"); }
void colorItalicBoldHighIntensityGreen() { printf("\033[3;1;92m"); }
void colorItalicBoldHighIntensityYellow() { printf("\033[3;1;93m"); }
void colorItalicBoldHighIntensityBlue() { printf("\033[3;1;94m"); }
void colorItalicBoldHighIntensityMagenta() { printf("\033[3;1;95m"); }
void colorItalicBoldHighIntensityCyan() { printf("\033[3;1;96m"); }
void colorItalicBoldHighIntensityWhite() { printf("\033[3;1;97m"); }

// Italic Standard Colors
void colorItalicBlack() { printf("\033[3;30m"); }
void colorItalicRed() { printf("\033[3;31m"); }
void colorItalicGreen() { printf("\033[3;32m"); }
void colorItalicYellow() { printf("\033[3;33m"); }
void colorItalicBlue() { printf("\033[3;34m"); }
void colorItalicMagenta() { printf("\033[3;35m"); }
void colorItalicCyan() { printf("\033[3;36m"); }
void colorItalicWhite() { printf("\033[3;37m"); }

// Italic Bright Colors
void colorItalicBrightBlack() { printf("\033[3;1;30m"); }
void colorItalicBrightRed() { printf("\033[3;1;31m"); }
void colorItalicBrightGreen() { printf("\033[3;1;32m"); }
void colorItalicBrightYellow() { printf("\033[3;1;33m"); }
void colorItalicBrightBlue() { printf("\033[3;1;34m"); }
void colorItalicBrightMagenta() { printf("\033[3;1;35m"); }
void colorItalicBrightCyan() { printf("\033[3;1;36m"); }
void colorItalicBrightWhite() { printf("\033[3;1;37m"); }

void resetColor() { printf("\033[0m"); }