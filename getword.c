/*
* Jaspar Deeb
* Takes input from stdin and inputs it into a array. Returns the length of the first word and the word itself in the driver p0. Ignores leading spaces.
*
* Return values:
* -10 if the first character is a new line.
* 0 if EOF is reached when the array is empty.
* Returns the negative of the length for metacharacters:
* "<", ">", "|", "&", "#", "|&"
* Backslash will escape any of these metacharacters and itself.
* If backslash does not preceed a metacharacter, it will not be printed.
* The first word length in all other cases.
*/
   
   #include "getword.h"
   int getword(char *w) {
     int x = 0;
     memset(w, 0, 255); //removes what's currently in the array (last output)
     w[x++] = getchar(); //starts getting the next few characters
     
     // ***** Leading Character section ******
     
     if(w[0] == 32) { //leading space reached
       for(;;) {
         w[x] = getchar();
         if(w[x] != 32) {
           ungetc(w[x], stdin); //puts the non-space back into the stream
           memset(w, 0, 255); //deletes all leading spaces
           x = 0;
           break; //continues the code outside loop
         } else {
           x++;
           }
       }
         w[x++] = getchar();
     }
     
     switch(w[0]) { //check the first character for metacharacters
       case EOF:
         w[0] = 0;
         return 0;
       case 10: // new line reached
         w[0] = 0;
         return -10;
       case 35: // # reached
         return -1;
       case 60: // < reached
         return -1;
       case 62: // > reached
         return -1;
       case 38: // & reached
         return -1;
       case 124: // | reached
         w[x] = getchar();
         if(w[x] == 38)  { // |& reached
         return -2;
         }
         else {
           ungetc(w[x], stdin); //the next character is not &, so we put it back on the stream
           w[x] = 0;
           return -1;
         }
       case 92: // backslash reached
         w[x] = getchar();
         if(w[x] == 35 || w[x] == 32 || w[x] == 60 || w[x] == 62 || w[x] == 38 || w[x] == 124) {
           w[x - 1] = w[x]; //replace backslash with the metacharacter and continue
         }
         else {
           ungetc(w[x--], stdin); //put regular character back on the stream and rid the backslash (--)
         }
       default:
         break;
     }
     
     // ***** Word Section ******
     
     for(;;) { //first word reached
       if(x >= STORAGE - 1) { //ensures that array is never overflowed
         return x;
         break;
       }
       w[x] = getchar();
       if(w[x] == 32 || w[x] == 60 || w[x] == 62 || w[x] == 38 || w[x] == 124) { // checks for metacharacters
         ungetc(w[x], stdin); //metacharacter put back on stream
         w[x] = 0; //terminate the word sequence and return the word
         return x;
       }
       switch(w[x]) { //checks for newline, backslash, and EOF for each char
         case EOF:
            w[x] = 0;
            return x;
         case 10: //newline
            ungetc(w[x], stdin);
            w[x] = 0;
            return x;
         case 92: //backslash
            w[++x] = getchar();
         if(w[x] == 32 || w[x] == 60 || w[x] == 62 || w[x] == 38 || w[x] == 124 || w[x] == 92) {
            w[x - 1] = w[x]; //backslash is replaced by the next character.
            x--; //Now the next character will be replaced anyway in the for loop.
         }
         else {
           ungetc(w[x--], stdin); //puts checked character back on the stream
           x--; //ensures the backslash is not printed
         }
      default:
      break;
      }
    x++;
  }
}
