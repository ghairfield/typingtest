# Typing test game

A work in progress.

Usage: spd 

Current todo list (in no order):
- [X] Move speedtest.[c|h] to screen.[c|h]
- [X] Python app to make the word list 
- [X] Character encoding that encodes the character and color values
- [X] Word list.
- [ ] Levels like tetris with more words faster
- [ ] ~~Refactor screen functions. Need special case for 80x20~~
- [X] Data structure for words on screen
- [X] ~Way to update screen only when necessary~ Frame buffer
- [X] Figure out location of game stats
- [ ] Save game? High score?
- [ ] Scale game if screen size changes
- [ ] Change game speed depending on size
- [X] Colors should change back to default after writing.

#### Reason
I wanted to make a game using low level terminal functions. This program is done
with out the help of _ncurses_ or other lib's. Just a fun test and nothing more.

## Program considerations

#### Character encoding
Currently I am considering using a uint16\_t for the character. The 4 most 
significant bits of the upper byte representing the foreground color. The
least 4 significant bits of the upper byte would encode the background color. 
The lower byte would represent the actual character value.  
Downside to this is the possibility of 8 extra bits per terminal character. If 
no color was used (white on black).
 
#### Data structure
The obvious choice would be an array that covers the screen. On an 80x20 screen
that would be 3200 bytes (3.125Kb), assuming the current 16 bit representation
of the character. 

#### Good resource
[VT100 Term Codes](https://termsys.demon.co.uk/vtansi.htm)
