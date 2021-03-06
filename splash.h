#ifndef SPLASH_H
#define SPLASH_H

// this is something like 1.7 KB - but it's read-only, so it doesn't count against code size.
#define splashText "\r\n SPLASH!"
/*"          _____                _____                    _____                    _____                    _____          \r\n"\
"         /\\    \\              /\\    \\                  /\\    \\                  /\\    \\                  /\\    \\         \r\n"\
"        /::\\    \\            /::\\    \\                /::\\    \\                /::\\    \\                /::\\____\\        \r\n"\
"       /::::\\    \\           \\:::\\    \\              /::::\\    \\              /::::\\    \\              /::::|   |        \r\n"\
"      /::::::\\    \\           \\:::\\    \\            /::::::\\    \\            /::::::\\    \\            /:::::|   |        \r\n"\
"     /:::/\\:::\\    \\           \\:::\\    \\          /:::/\\:::\\    \\          /:::/\\:::\\    \\          /::::::|   |        \r\n"\
"    /:::/__\\:::\\    \\           \\:::\\    \\        /:::/__\\:::\\    \\        /:::/__\\:::\\    \\        /:::/|::|   |        \r\n"\
"   /::::\\   \\:::\\    \\          /::::\\    \\      /::::\\   \\:::\\    \\      /::::\\   \\:::\\    \\      /:::/ |::|   |        \r\n"\
"  /::::::\\   \\:::\\    \\        /::::::\\    \\    /::::::\\   \\:::\\    \\    /::::::\\   \\:::\\    \\    /:::/  |::|___|______  \r\n"\
" /:::/\\:::\\   \\:::\\____\\      /:::/\\:::\\    \\  /:::/\\:::\\   \\:::\\    \\  /:::/\\:::\\   \\:::\\____\\  /:::/   |::::::::\\    \\ \r\n"\
"/:::/  \\:::\\   \\:::|    |    /:::/  \\:::\\____\\/:::/__\\:::\\   \\:::\\____\\/:::/  \\:::\\   \\:::|    |/:::/    |:::::::::\\____\\\r\n"\
"\\::/   |::::\\  /:::|____|   /:::/    \\::/    /\\:::\\   \\:::\\   \\::/    /\\::/   |::::\\  /:::|____|\\::/    / ~~~~~/:::/    /\r\n"\
" \\/____|:::::\\/:::/    /   /:::/    / \\/____/  \\:::\\   \\:::\\   \\/____/  \\/____|:::::\\/:::/    /  \\/____/      /:::/    / \r\n"\
"       |:::::::::/    /   /:::/    /            \\:::\\   \\:::\\    \\            |:::::::::/    /               /:::/    /  \r\n"\
"       |::|\\::::/    /   /:::/    /              \\:::\\   \\:::\\____\\           |::|\\::::/    /               /:::/    /   \r\n"\
"       |::| \\::/____/    \\::/    /                \\:::\\   \\::/    /           |::| \\::/____/               /:::/    /    \r\n"\
"       |::|  ~|           \\/____/                  \\:::\\   \\/____/            |::|  ~|                    /:::/    /     \r\n"\
"       |::|   |                                     \\:::\\    \\                |::|   |                   /:::/    /      \r\n"\
"       \\::|   |                                      \\:::\\____\\               \\::|   |                  /:::/    /       \r\n"\
"        \\:|   |                                       \\::/    /                \\:|   |                  \\::/    /        \r\n"\
"         \\|___|                                        \\/____/                  \\|___|                   \\/____/     "
*/

#define faultText "\r\n"\
"--------------------------------------------------------------------------------\r\n"\
"  *  CRITICAL ERROR  *  CRITICAL ERROR  *  CRITICAL ERROR  *  CRITICAL ERROR  * \r\n"\
"--------------------------------------------------------------------------------\r\n"\
"\r\n          HARD FAULT!!!\r\n\r\n\a"

#define fatalErrorText "\r\n"\
"--------------------------------------------------------------------------------\r\n"\
"    *   FATAL ERROR   *   FATAL ERROR   *   FATAL ERROR   *   FATAL ERROR   *   \r\n"\
"--------------------------------------------------------------------------------\r\n"

#endif
