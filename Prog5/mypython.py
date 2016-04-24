#!/usr/bin/python
# Program 5
# Diana O'Haver

import random
import string

# random character generator adapted from: https://pythontips.com/2013/07/28/generating-a-random-string/

def random_generator(size=10, chars=string.ascii_lowercase):
	return ''.join(random.choice(chars) for x in range(size))

rand1 = random_generator()
rand2 = random_generator()
rand3 = random_generator()

file1 = open("file1.txt", 'w')
file1.write(rand1 + "\n")

file2 = open("file2.txt", 'w')
file2.write(rand2 + "\n")

file3 = open("file3.txt", 'w')
file3.write(rand3 + "\n")

file1.close()
file2.close()
file3.close()

print "File 1: " + rand1
print "File 2: " + rand2
print "File 3: " + rand3

randint1 = random.randrange(1, 42)

randint2 = random.randrange(1, 42)

sum = randint1 + randint2

print str(randint1) + " + " + str(randint2) + " = " + str(sum)


