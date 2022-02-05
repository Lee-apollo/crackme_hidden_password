
import string
import random


def calc_checksum(ascii_password):
    return hex(sum([ord(char) * i for i, char in enumerate(ascii_password)]) + 0x7FFFFFFF)

required_value = calc_checksum("hello_world_42")
assert(required_value == '0x8000203d')

# All printable ASCII characters are stored in string.printable, but it also contains chars like \t, \r, \n. Because of this combination of string.ascii_letters, string.punctuation and string.digits is used.
def generate_password():
    pass_len = random.randint(1,20)
    return "".join([random.choice(string.ascii_letters + string.punctuation + string.digits) for i in range(pass_len)])

#while 1:
#    password = generate_password()
#    if required_value == calc_checksum(password):
#        print("Password: ", password)

checksum = int(required_value, 16)

#checksum = 150

modulo_result = dict()

for i in (123, 456, 789, 987, 654, 321):
    modulo_result[hex(i)] = hex(checksum % i)

print(modulo_result)
exit(0)

possible_values = {}

for first in modulo_result.keys():
    for second in modulo_result.keys():
        possible_values[second] = []
        if first == second:
            continue

        for number in range(0x80000000, 0x90000000):
            if ((number * int(second) + modulo_result[second] - + modulo_result[first]) % int(first)) == 0:
                possible_values[second].append(number)


for i in possible_values:
    print(i, possible_values[i])

candidates = {}

for first in possible_values.keys():
    candidates[first] = set()
    for second in possible_values.keys():
        if first == second:
            continue
        
        for i in possible_values[first]:
            for j in possible_values[second]:
                result_A = i * int(first) + modulo_result[first]
                result_B = j * int(second) + modulo_result[second]
                if result_A == result_B:
                    print("Candidate:", result_A)
                    candidates[first] |= {result_A}

print("Results:")

result = None

for key in candidates.keys():
    if result == None:
        result = candidates[key]
    print(sorted(candidates[key]))
    if len(candidates[key]) == 0:
        continue
    result = candidates[key] & result

print(result)
#for number in range(0, 100000000):
#
#    if valid:
#        print(hex(number))
#
#
#
#
#
#for number in range(0, 0x90000000):
#    valid = True
#    if number % 0x100000 == 0:
#        print("Tralala")
#    for key in modulo_result.keys():
#        valid &= (number % int(key) == modulo_result[key])
#
#    if valid:
#        print(hex(number))