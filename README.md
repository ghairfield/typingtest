# Typing test game

A work in progress.
> 14-Nov-2019: Doing some updates. Would like to make the words move from left
to right across the screen. 

Usage: spd 

Keys:
  ```
  CTRL-C / Q: Quite the game  
  Escape    : Go back a screen  
  M         : Game settings  
  L         : Load a typing file (todo)  
  S         : Start game  
  C         : Conways game of life (why not)  
  ```
Current todo list (in no order):
- [ ] Move speedtest.[c|h] to screen.[c|h]
- [ ] Python app to make the word list 
- [ ] Character encoding that encodes the character and color values
- [ ] Word list. Easy, medium, hard
- [ ] Refactor screen functions
- [ ] Data structure for words on screen
- [ ] Way to update screen only when necessary
- [ ] Figure out location of game stats
- [ ] Save game? High score?

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

