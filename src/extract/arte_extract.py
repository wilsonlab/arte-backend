#! /usr/bin/python
import sys
import os

print "Usage for Data/rat/110812: ~/arte-ephys/src/extract/arte_extract.py -trodenames 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 -trodelabels 1411 1311 1211 1011 0911 0811 0711 0611 0511 0411 0211 0111 tt1811 tt1711 tt1611 tt1511 -lfpbanknames 0 1 -i raw/arte110812.data"

num_list = []
label_list = []
beg_index = len(sys.argv)
list_end = len(sys.argv) - 1
beg_label_index = len(sys.argv)
label_list_end = len(sys.argv) - 1
trode_label_prefix = ""
beg_lfp_index = len(sys.argv)
end_lfp_index = len(sys.argv) - 1
lfp_list = []

for i in range(len(sys.argv)):
   if sys.argv[i] == '-trodenames':
      beg_index = i+1
   elif sys.argv[i][0]=='-':
       trode_list_end  = i
       break

for i in range(len(sys.argv)):
   if sys.argv[i] == "-trodelabels":
      beg_label_index = i+1
   elif (sys.argv[i][0] == '-') & (i > beg_label_index):
      label_list_end = i
      break

for i in range(len(sys.argv)):
   if sys.argv[i] == "-trodelabelprefix":
      trode_label_prefix = sys.argv[i+1]
   else:
      trode_label_prefix = ""

for i in range(len(sys.argv)):
   if sys.argv[i] == "-lfpbanknames":
      beg_lfp_index = i+1
   elif (sys.argv[i][0] == '-') & (i > beg_lfp_index):
      end_lfp_index = i
      break

for i in range(len(sys.argv)):
   print(sys.argv[i])
   if sys.argv[i] == "-i":
      input_filename = sys.argv[i+1]
#   else:
#      input_filename = "artedate.data"


for i in range(beg_lfp_index, end_lfp_index):
   lfp_list.append(int(sys.argv[i]))

for i in range(beg_index, trode_list_end):
   num_list.append(int(sys.argv[i]))

for i in range(beg_label_index, label_list_end+1):
   label_list.append(sys.argv[i])

for i in range(0, len(num_list)):
   print str(i) + ": num_list=" + str(num_list[i]) + " label_list=" + label_list[i]

dir_list = []
for i in range(0, len(num_list)):

   dir_list.append(trode_label_prefix + label_list[i])
#   if i < 10:
#       dir_list.append(trode_label_prefix + '0' + trode_labels)
#   else:
#       dir_list.append(trode_label_prefix + str(i))
#print dir_list

for i in dir_list:
   if not os.path.exists(i):
       os.makedirs(i)


for i in range(len(num_list)):
    cmd_path = "arte_to_mwl"
    final_cmd = cmd_path + " -i " + input_filename + " -o ./" + (dir_list[i]) + "/" + (dir_list[i]) + ".tt -trodename " + str(num_list[i])
    spikeparms_cmd = "spikeparms2 ./" +  (dir_list[i]) + "/" + (dir_list[i]) + ".tt -tetrode -binary -parms t_px,t_py,t_pa,t_pb,t_maxwd,t_maxht,time -pos *.p -o " + (dir_list[i]) + "/" + (dir_list[i]) + ".pxyabw"
    os.system(final_cmd)
    os.system(spikeparms_cmd)

for i in range(len(lfp_list)):
   cmd_path = "arte_to_mwl"
   final_cmd = cmd_path + " -i " + input_filename + " -o ./arte_lfp" + str(lfp_list[i]) + ".eeg -lfpbankname " + str(lfp_list[i])
   os.system(final_cmd)
