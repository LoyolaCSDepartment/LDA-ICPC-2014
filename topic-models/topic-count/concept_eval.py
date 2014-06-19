#!/usr/bin/env /usr/bin/python3

import cProfile
import datetime
import math
import os
import subprocess
import sys
import re 

PROFILE = False
VERBOSE = False

if len(sys.argv) < 4:
  print("usage: " + sys.argv[0] + " <input split file directory> <project name> <trainign iterations> [<samples>]")
  sys.exit (-1)

INPUT_DIR = sys.argv[1] + '/'  # redundant ok; without sucks!
PROJECT_NAME = sys.argv[2]
TRAINING_ITERATIONS = sys.argv[3]

# SAMPLES only used by fake-gibbs when calling hacked mallet 
# that outputs intermediate models
if len(sys.argv) == 4:
  SAMPLES = sys.argv[4]
else:
  SAMPLES = 1

# print("called with " , INPUT_DIR ," ", PROJECT_NAME ," ", TRAINING_ITERATIONS ," ", SAMPLES)
# sys.exit (-1)


# values originally used
# TOPIC_COUNTS = [25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300]
TOPIC_COUNTS = [5, 10, 25, 50, 75, 100, 150, 200, 300]
# TOPIC_COUNTS = [8, 12, 18]  # for tool chain testing
# sys.stderr.write('*** Warning concept_eval.py in tool chain test mode!!\n')
# TOPIC_COUNTS = [12]  # for tool chain testing

# 6/23 now passed in
# TRAINING_ITERATIONS = 1000000   
# TRAINING_ITERATIONS = 2000   # FIX ME use in production 
# TRAINING_ITERATIONS = 1000

NUM_MATCHES = [5, 10, 25, 50]

if PROFILE:
  print('PROFILE MODE..  ACTIVATED!!')

def getFilenameByIndex(x):
  try:
    input_file = open('{0}{1}.txt'.format(INPUT_DIR,x))
  except IOError:
    return None
  input_st = input_file.readline()
  input_st = input_st[input_st.find('"')+1:]
  input_st = input_st[:input_st.find('"')]
  input_file.close()
  return input_st

# omit 'suffix_omit' parts from the tail of path names x and y
def getFilenameMatches(x, y, suffix_omit):
  if suffix_omit == 0:
    return x == y
  xs = x.split('/')
  ys = y.split('/')
  while len(xs) > len(ys):
    ys = ys + ['@@']
  while len(ys) > len(xs):
    xs = xs + ['@@']
  return xs[:-suffix_omit] == ys[:-suffix_omit]


def getSecondFromList(x):
  return x[1]


def getFloatList(list_obj):
  ret_obj = []
  for x in list_obj:
    ret_obj.append(float(x))
  return ret_obj


def getCosine(x, y, length, x_mag, y_mag):
  dp = 0
  for i in range(length):
    dp += x[i] * y[i]
  cosine = dp / x_mag * y_mag
  return cosine


def runLda(topics):
   lda_cmd = '/research/home/joverfel/optimal-concepts/GibbsLDA++-0.2/src/lda -est -ntopics {0} '.format(topics) \
        + '-dfile  ' +INPUT_DIR+'/lda.txt -twords 20 -niters {0}'.format(TRAINING_ITERATIONS)
  print(lda_cmd)
  p = subprocess.Popen(lda_cmd, shell=True, stdout=open('/dev/null', 'w'))
  sts = os.waitpid(p.pid, 0)


def saveNearestNeighboursFast(model_file):
  #input_file = open('{0}model-final.theta'.format(INPUT_DIR), 'r')
  input_file = open('{0}{1}'.format(INPUT_DIR, model_file), 'r')
  output_file = open('{0}nn.txt'.format(INPUT_DIR), 'w')
  vector_pos = 0
  vector_obj = {}
  vector_obj_mag = {}
  for line in input_file:
    vector_pos += 1
    list_obj = getFloatList(line.strip().split(' '))
    vector_obj[vector_pos] = list_obj
    x_mag = 0
    for i in range(len(list_obj)):
      x_mag += list_obj[i] * list_obj[i]
    x_mag = math.sqrt(x_mag)
    vector_obj_mag[vector_pos] = x_mag
  input_file.close()
  for x in range(1, len(vector_obj) + 1):
    list_length = len(vector_obj[x])
    cosine_obj = []
    for y in range(1, len(vector_obj) + 1):
      nn_cosine = getCosine(vector_obj[x], vector_obj[y], list_length,
          vector_obj_mag[x], vector_obj_mag[y])
      cosine_obj.append((y, nn_cosine))
    cosine_sorted = sorted(cosine_obj, key=getSecondFromList, reverse=True)
    cosine_sorted = cosine_sorted[0:max(NUM_MATCHES)]
    match_list = []
    for (index, cosine) in cosine_sorted:
      match_list.append('{0}={1}'.format(index, cosine))
    output_file.write(','.join(match_list))
    output_file.write('\n')
    if x % 10000 == 0:
      print(datetime.datetime.now(), ':    ', x,
            ' nearest neighbours counted', sep='')
  output_file.close()


def saveNNResults(input_name, output_name, suffix_omit, tag_map):
  input_file = open('{0}{1}'.format(INPUT_DIR, input_name), 'r')
  output_file = open('{0}{1}'.format(INPUT_DIR, output_name), 'w')
  index = 0
  for line in input_file:
    index += 1
    results_obj = []
    list_obj = line.strip().split(',')
    for size in NUM_MATCHES:
      match_count = 0
      list_obj_trim = list_obj[:size]
      for x in list_obj_trim:
        id = int(x[0:x.find('=')])
        if getFilenameMatches(tag_map[index], tag_map[id], suffix_omit):
          match_count += 1
      results_obj.append('{0},{1}'.format(size, match_count))
    output_file.write('{0}\n'.format(' '.join(results_obj)))
  output_file.close()
  input_file.close()


def saveNNTopicResults(input_name, output_name, suffix_omit, tag_map):
  input_file = open('{0}{1}'.format(INPUT_DIR, input_name), 'r')
  output_file = open('{0}{1}'.format(INPUT_DIR, output_name), 'w')
  index = 0
  for line in input_file:
    list_obj = line.strip().split(',')
    results_obj = []
    for size in NUM_MATCHES:
      match_count = 0
      id_obj = []
      list_obj_trim = list_obj[:size]
      for x in list_obj_trim:
        id_obj.append(int(x[0:x.find('=')]))
      for x in id_obj:
        for y in id_obj:
          if getFilenameMatches(tag_map[x], tag_map[y], suffix_omit):
            match_count += 1
      results_obj.append('{0},{1}'.format(size, match_count))
    output_file.write('{0}\n'.format(' '.join(results_obj)))
  output_file.close()
  input_file.close()

def getPrecisionAverages(input_name):
  results_obj = []
  for size_pos in range(len(NUM_MATCHES)):
    input_file = open('{0}{1}'.format(INPUT_DIR, input_name), 'r')
    index = 0
    sum = 0
    for line in input_file:
      index += 1
      line_obj = line.strip().split(' ')
      list_obj = line_obj[size_pos].strip().split(',')
      sum += float(list_obj[1])
    input_file.close()
    results_obj.append( (NUM_MATCHES[size_pos], sum / index))
  return results_obj


def getTopicAverages(topics, model_file):
  output_file = open('{0}nn_topic.txt'.format(INPUT_DIR), 'w')
  for x in range(topics):
    # input_file = open('{0}model-final.theta'.format(INPUT_DIR), 'r')
    input_file = open('{0}{1}'.format(INPUT_DIR,model_file), 'r')
    index = 0
    topic_obj = []
    for line in input_file:
      index += 1
      list_obj = getFloatList(line.strip().split(' '))
      topic_obj.append((index, list_obj[x]))
    input_file.close()
    topic_sorted = sorted(topic_obj, key=getSecondFromList, reverse=True)
    topic_sorted = topic_sorted[0:max(NUM_MATCHES)]
    match_list = []
    for (index, value) in topic_sorted:
      match_list.append('{0}={1}'.format(index, value))
    output_file.write(','.join(match_list))
    output_file.write('\n')
  output_file.close()


def build_name_file_map(dir):
  map = []
  index = 0
  max_length = 0
  name = "entry 0 unused"
  while (name != None):
    map.insert(index, name)
    l = len(name.split('/'))
    if l > max_length:
      max_length = l
    index += 1
    name = getFilenameByIndex(index)
  return (map, max_length+1)


def main():
  base_dir = os.getcwd()
  os.chdir(INPUT_DIR)

  (tag_map , max_length) = build_name_file_map(INPUT_DIR)
  print(datetime.datetime.now(),
      ': init, max length {0}..'.format(max_length), sep='')

  # max_length = 1 # FIX ME omit in production ... testing override

  doc_results = []
  topic_results = []
  for ii in range(max_length):
    doc_results.insert(ii, {})
    topic_results.insert(ii, {})
    for tc in TOPIC_COUNTS:
      doc_results[ii][tc] = {}
      topic_results[ii][tc] = {}
      for nm in NUM_MATCHES:
        doc_results[ii][tc][nm] = 0
        topic_results[ii][tc][nm] = 0

  movie_data = open('movie_data_{0}.txt'.format(PROJECT_NAME), 'w') 
  movie_data.write('#Frame topic_count suffix_remove number_matches average_score')

  for tc in TOPIC_COUNTS:
    print(datetime.datetime.now(),
        ': Starting LDA with {0} topics..'.format(tc), sep='')
    runLda(tc)
    file_count = 0;
    for model_file in os.listdir('{0}'.format(INPUT_DIR)):
      if re.match('model-final.*.theta', model_file) == None:
        continue
      file_count += 1
      ##print(datetime.datetime.now(), ': Getting nearest neighbours..', sep='')
      saveNearestNeighboursFast(model_file)
      for ii in range(max_length):
        print(datetime.datetime.now(), 
              ': Starting pass {0} with model {1}.'.format(ii, file_count), sep='')
        ##print(datetime.datetime.now(), ': Saving relationships..', sep='')
        saveNNResults('nn.txt', 'nn_results.txt', ii, tag_map)
        ##print(datetime.datetime.now(), ': Getting document averages..', sep='')
        results_obj = getPrecisionAverages('nn_results.txt')
        for result in results_obj:
          (num_matches, avg) = result
          # average_doc_file[ii].write('{0},{1},{2}\n'.format(tc, num_matches, avg))
          # print('{0},{1},{2}'.format(tc, num_matches, avg), sep='')
          doc_results[ii][tc][num_matches] += avg
    
        ##print(datetime.datetime.now(), ': Saving topic relationships..', sep='')
        getTopicAverages(tc, model_file)
        saveNNTopicResults('nn_topic.txt', 'nn_topic_results.txt', ii, tag_map)
        ##print(datetime.datetime.now(), ': Getting topic averages..', sep='')
        results_obj = getPrecisionAverages('nn_topic_results.txt')
        for result in results_obj:
          (num_matches, avg) = result
          # average_topic_file[ii].write('{0},{1},{2}\n'.format(tc, num_matches, avg))
          # print('{0},{1},{2}'.format(tc, num_matches, avg), sep='')
          topic_results[ii][tc][num_matches] += avg

        # for movie of developing averages
        # print('Frame {0} for topic count {1}'.format(file_count, tc))
        for nm in NUM_MATCHES:
          ave = topic_results[ii][tc][nm] / file_count
          # ave = doc_results[ii][tc][nm] / file_count
          movie_data.write('\n{0} {1} {2} {3} {4}'.format(file_count, tc, ii, nm, ave))
  
  movie_data.close()


  print('CROSS CHECK file count = {0}'.format(file_count), sep='')
  for ii in range(max_length):
    average_doc_file = open('average_{0}_doc_{1}.txt'.format(PROJECT_NAME, ii), 'w') 
    average_topic_file = open('average_{0}_topic_{1}.txt'.format(PROJECT_NAME, ii), 'w') 

    for tc in TOPIC_COUNTS:
      for nm in NUM_MATCHES:
        ave = doc_results[ii][tc][nm] / file_count
        # print('{0},{1},{2}'.format(tc, nm, ave)) 
        average_doc_file.write('{0},{1},{2}\n'.format(tc, nm, ave))
    for tc in TOPIC_COUNTS:
      for nm in NUM_MATCHES:
        ave = topic_results[ii][tc][nm] / file_count
        # print('{0},{1},{2}'.format(tc, nm, ave))
        average_topic_file.write('{0},{1},{2}\n'.format(tc, nm, ave))

    average_doc_file.close()
    average_topic_file.close()

  os.chdir(base_dir)
  print(datetime.datetime.now(), ': All done!', sep='')


print(datetime.datetime.now(), ': Starting up..', sep='')
if __name__ == "__main__":
  if PROFILE:
    cProfile.run('main()')
  else:
    main()
