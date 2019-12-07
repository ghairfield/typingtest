#!/usr/bin/python3
import random
import sys

def extract_words(wl, wo='words.txt', small=4, large=12):
    out = []
    fp = open(wl, 'r')
    for line in fp:
        length = len(line)
        if length > small and length < large:
            out.append(line)
    fp.close()
    random.shuffle(out)
    fout = open(wo, 'w')
    # Write some file information for spd to read
    # Number of words, smallest word, largest word
    fout.write('#' + str(len(out)) + ';' + str(small) + ';' + str(large) + '\n');
    for index in out:
        fout.write(index[:-1] + ';')
    fout.close()

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
