// LCD Menu 
#define PROGRAM_VERSION         "LCDMenu v0.5"

#define TITLE_BAR_HEIGHT        40
#define TITLE_BAR_COLOR         0xff1253d6
#define TITLE_BAR_TEXTCOLOR     0xffffffff

#define STATUS_BAR_HEIGHT       30
#define STATUS_BAR_COLOR        0xff000000
#define STATUS_BAR_TEXTCOLOR    0xffaaaaaa

#define MENU_FRAME_COLOR        0xff1f537f
#define MENU_FRAME_TEXTCOLOR    0xffffffff
#define MENU_FRAME_LINE_HEIGHT  35
#define MENU_FRAME_PADDING      10

#define MENU_DISPLAY_COUNT       7
#define MENU_HIGHLIGHT_TEXTCOLOR 0xffffffff
#define MENU_HIGHLIGHT_BACKCOLOR 0xffe95431

const char *MenuOptions[] = {
    "1 Menu One",
    "2 Menu Two",
    "3 Menu Three",
    "4 Menu Four",
    "5 Menu Five",
    "6 Menu Six",
    "7 Menu Seven",
    "8 Menu Eight",
    "9 Menu Nine",
    "10 Menu Ten",
    "11 Menu Eleven",
    "12 max 255 menu",
    "13 or more...",
    "14 More items",
    "15 possible :)",
    "16 just add to",
    "17 array items.",
    "18 We can go on",
    "19 forever here."};


uint8_t mainMenuOffset = 0;
uint8_t mainMenuPosition = 1;
uint8_t totalMenuCount = sizeof(MenuOptions) / sizeof(char *);

// forward declare functions
void DrawTitleBar(char *Title);
void ClearStatusBar();
void UpdateStatusBar(char *Text);
void DrawMenuFrame();
void UpdateDisplayMenu(uint8_t menuOffset, uint8_t position);
void DisplayMenuOptions(uint8_t menuOffset);
void HighlightMenuOption(uint8_t menuOffset, uint8_t position);