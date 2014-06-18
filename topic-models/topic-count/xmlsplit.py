#! /usr/bin/env /usr/bin/python3

import os
import sys

def ensure_dir(f):
  d = os.path.dirname(f)
  if not os.path.exists(d):
    os.makedirs(d)

def xmlSplit(infile_name, dest_dir):
  try:
    # in_file = open('{0}{1}'.format(folder, filename), 'r', encoding='latin_1')
    in_file = open(infile_name, 'r', encoding='latin_1')
  except IOError:
    print("File not found.")
    return

  dest_dir += '/'  # redundant ok; without sucks!
  # dest_dir = '{0}input/'.format(folder)
  ensure_dir(dest_dir)
  file_num = 1

  out_file = open('%s%d.txt' % (dest_dir, file_num), 'w')
  file_open = True
  for x in in_file:
    if x[-1] != '\n':
      x = '%s\n' % (x)

    if not file_open:
        file_open = True
        out_file = open(next_file, 'w')

    # hack to remove non-ascii characters
    x = ''.join([c for c in x if ord(c) < 128])
    out_file.write('%s' % (x))

    if x.startswith('</source>'):
      out_file.close()
      file_num += 1
      next_file = '%s%d.txt' % (dest_dir, file_num)
      file_open = False

  print('{0} files'.format(file_num - 1) + " left in " + dest_dir)
  out_file.close()
  in_file.close()

if len(sys.argv) != 3:
  print("usage: " + sys.argv[0] + " <input xml file> <output directory>")
  sys.exit (-1)

xmlSplit(sys.argv[1], sys.argv[2])
# example call: xmlsplit.py cook.xml /scratch/topics/out')
# xmlSplit('<FIX ME>/topic-models/topic-count/sources/', 'cook.xml')
