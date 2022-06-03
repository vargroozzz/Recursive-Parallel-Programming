import sys
import random

MAX_VALUE = 1000000
MIN_VALUE = -1000000

if __name__ == '__main__':
    filename = sys.argv[1]
    rows = int(sys.argv[2])
    columns = int(sys.argv[3])
    file = open(filename, 'w')
    file.write(f'[{rows}, {columns}](')
    for i in range(rows):
        file.write('(')
        for j in range(columns):
            file.write(str(random.randint(MIN_VALUE, MAX_VALUE)))
            if j != columns - 1:
                file.write(', ')
            else:
                file.write(')')

        if i != rows - 1:
            file.write(', ')
        else:
            file.write(')')

    file.close()
