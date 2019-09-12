from pathlib import Path
import string 


p = Path('test')
tmp = [x for x in p.iterdir()]

op_txt = list()
index = 1
for i in tmp:
    file_obj = open(i, "r")
    a = file_obj.read()
    print(len(a))
    # print(a.split("\n")[2])
    op_txt.append("file "+str(index))
    index+=1
    tmp1 = a.split("\n")[2].translate(str.maketrans('', '', string.punctuation))
    clean_string = ''
    for i in tmp1:
        if(i.isalpha() or i == ' '):
            clean_string += i
    op_txt.append(clean_string)

        # s.translate(str.maketrans('', '', string.punctuation))

    # for j in a.split("\n"):
    #     print(j)
    # print(a.split("\n"))
    # break
# print(tmp)

with open('clean_test.txt', 'w') as f:
    for item in op_txt:
        f.write("%s\n" % item)