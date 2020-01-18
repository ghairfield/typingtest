#!/usr/bin/python3
import random
import sys

'''
def extract_words(wl, wo='words.txt', small=4, large=12):
    random.shuffle(out)
    fout = open(wo, 'w')
    # Write some file information for spd to read
    # Number of words, smallest word, largest word
    fout.write('# Basic word list from dictionary;')
    fout.write('#' + str(len(out)) + ';' + str(small) + ';' + str(large) + '\n')
    for index in out:
        fout.write(index[:-1] + ';') # Don't want '\n' in word
    fout.close()
'''

# Words should come here already shuffled.
def create_word_list(words, wo='words.txt', size=500):
    fp = open(wo, 'w')

    for i in range(size):
        fp.write(random.choice(words) + '\n')     
    fp.close()


# Open a file and read in the words. This file should be 
# a master word file.
def read_file(wl, small=4, large=12):
    out = []
    fp = open(wl, 'r')

    for line in fp:
        length = len(line)
        if length > small and length < large:
            out.append(line[:-1]) # Remove trailing newline
    fp.close()
    return out

if __name__ == '__main__':
    l = read_file('words_alpha.txt')
    create_word_list(l)

'''
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: ', sys.argv[0], 'filename [outputfile] [smallest largest]\n');
        exit(0)
    if len(sys.argv) == 2:
        fname = sys.argv[1]
        extract_words(fname)
    elif len(sys.argv) == 3: 
        fname = sys.argv[1]
        fout  = sys.argv[2]
        extract_words(fname, fout)
    elif len(sys.argv) == 5:
        fname = sys.argv[1]
        fout  = sys.argv[2]
        small = int(sys.argv[3])
        large = int(sys.argv[4])
        extract_words(fname, fout, small, large)
'''
