#! /usr/bin/env /usr/bin/python3
from datetime import datetime

print(datetime.now(), ': Starting up..', sep='')

import math
import os
import sys
import re

def getFirstLineOfFile(x):
  input_file = open(x, 'r')
  input_st = input_file.readline()
  input_file.close()
  return input_st.strip()

def keyGetFilenameInt(x):
  tail = os.path.split(x)[1]
  return int(tail[0:tail.find('.')])

def getFileList(folder):
  file_list = []
  pat = re.compile('\d*.txt')
  for (_, _, files) in os.walk(folder):
    for file in files:
      matchObj = pat.match(file)
      if matchObj:
        file_list.append(folder + file)
  return sorted(file_list, key=keyGetFilenameInt)

def stripStr(st):
  bad_words = ['</source>',
               '*','(',')',';','=','>','<','|','&',
               ',','[',']','%','$','"','{','}',':',"\n","'",'-',
               '~','!','\\n','.','+','?','\\r'
               ]
  st = st.lower()
  st = st.replace('#','');
  st = st.replace('`','');
  st = st.replace('@',' ');
  st = st.replace('^',' ');
  st = st.replace('/',' ');
  if (st.find('">') >= 0):
    st = st[st.find('">')+3:]
  for word in bad_words:
    st = st.replace(word, ' ')
  words = st.split()
  new_words = []
  for word in words:
# 5/12/13 dawn and i decided to include all words
#   if len(word) > 3:
    if not word[0].isdigit():
      new_words.append(word)
  return ' '.join(new_words)

def writeLdaInput(file_list, output_dir):
  print(datetime.now(), ': Starting output..', sep='')
  print(datetime.now(), ': ', len(file_list), ' files..', sep='')
  out_file = open('{0}lda.txt'.format(output_dir), 'w', encoding='latin_1')
  out_file.write('{0}\n'.format(str(len(file_list))))

  for input_file_name in file_list:
    in_file = open(input_file_name, 'r', encoding='latin_1')
    lines = in_file.readlines()
    st = ''
    for line in lines:
      st = '%s %s' % (st, stripStr(line))
    st = st.strip()
# 5/12/13 unsure why outputing 'aaa' is helpful / needful
#     if len(st) == 0:
#      st = 'aaa'
#    out_file.write('{0}\n'.format(st))
    if len(st) != 0:
      out_file.write('{0}\n'.format(st))
    in_file.close()

  out_file.close()
  print(datetime.now(), ': All done!', sep='')

def main():
  if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + " <directories with split files>")
    sys.exit (-1)


  for x in sys.argv[1:]:
    print(datetime.now(), ': PROCESSING ', x, sep='')
    input_dir = '{0}/'.format(x)
    file_list = getFileList(input_dir)
    id_file = open('{0}file_id.txt'.format(input_dir), 'w')
    for i in range(len(file_list)):
      id_file.write('{0},{1}\n'.format(i+1, getFirstLineOfFile(file_list[i])))
    id_file.close()
    writeLdaInput(file_list, input_dir)

if __name__ == "__main__":
  main()

