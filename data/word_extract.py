import random

def extract_words(wl='words_alpha.txt', wo='words.txt'):
    out = []
    fp = open(wl, 'r')
    for line in fp:
        length = len(line)
        if length > 4 and length < 12:
            out.append(line)
    fp.close()
    random.shuffle(out)
    fout = open(wo, 'w')
    # Write some file information for spd to read
    # Number of words, smallest word, largest word
    fout.write('#' + str(len(out)) + ';4;12\n');
    for index in out:
        fout.write(index[:-1] + ';')
    fout.close()

if __name__ == '__main__':
    extract_words()
