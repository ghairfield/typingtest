#!/usr/bin/python3
import random
import sys

def create_files(words, count, size, fileprefix):
    for i in range(count):
        fo = open(fileprefix + str(i) + '.txt', 'w')
        for pos in range(size):
            fo.write(words[pos + (size * i)])
        fo.close()


def create_word_list(words, size=1000, fileprefix='words'):
    files = int(len(words) / size);
    index = 1

    ans = input ('Can create ' + str(files) + ' word files. Continue? (Y/N)')
    if ans == 'Y' or ans == 'y':
        ans = input ('How many would you like to ceate? (1 .. ' + str(files) + ')')
        if int(ans) > 0 and int(ans) <= files:
            random.shuffle(words)
            create_files(words, int(ans), size, fileprefix)


def read_file(wl, small=4, large=12):
    out = []
    fp = open(wl, 'r')

    for line in fp:
        length = len(line)
        if length > small and length < large:
            out.append(line)
    fp.close()

    print(str(len(out)) + ' words meet the criteria (Small: ' + str(small) + \
                          ' Large: ' + str(large) + ')')
    return out


if __name__ == '__main__':
    l = read_file('masterwl.txt')
    create_word_list(l)
