/*
 * Homework 2 requirements:
 * A function puts that prints a NULL-terminated string to the terminal.
 * Calling puts sequentially will print strings one after another. In other words, puts should not overwrite text that was   previously written to the screen.
 * Printing a '\n' character should advance to the next temp_pos_x on the screen.
 * When the cursor gets to the bottom of the screen, advancing to a new temp_pos_x should scroll the existing text on the screen up one temp_pos_x.
 * */

/*
 * Note to myself:
 * Each of the memory location in the buffer is two byte.
 * char is 1 byte, int is 2 or 4 bytes, short is two bytes, long is 4 byte.
 * If declare the screen buffer as char pointer, i need to (pos_y * 80 + pos_x) * 2.
 *
 * If declare the screen buffer as short pointer like short *buffer = (short *)0xb8000
 * short *buffer = (short *) 0xb8000;
 * buffer[location] = c | (7<<8);
 * */

int pos_x, pos_y;
char *buffer = (char *) 0xb8000;

// move cursor
void move_cursor(){
    if (pos_x < 79){
        pos_x = pos_x + 1;
    } else {
        pos_y = pos_y + 1;
        pos_x = 0;
    }
}


void memory_copy(char *d, char *s, int n){
    while((n--) > 0){
        *(d++) = *(s++);
    }
}


//scroll the screen up
void scroll_up(){
    if (pos_y >= 25) {
        memory_copy((char *)0xb8000, (char* )0xb8000 + 160, 4000);
        pos_y = 24;
        pos_x = 0;
        while (pos_x < 160){
            buffer[pos_y*160 + pos_x] = ' ';
            pos_x += 2;
        }
        pos_y = 23;
	pos_x = 0;
    }
}


// print a ascii character to the screen
void putascii(char c){
    int location;
    location = (pos_y * 80 + pos_x) * 2; // should be two byte
    buffer[location] = c;
    *(buffer + 1) = 0x07;
}


//print any character
int putc(char c){
    if (c == '\n'){
        pos_x = 0;
        pos_y = pos_y + 1;
    } else {
        putascii(c);
        move_cursor();
    }
    scroll_up();

    return 1;
}


//print a string
void puts(char *c){
    int i = 0;
    while (c[i]) {
        putc(c[i++]);
    }
}

