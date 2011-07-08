import sys
import os

num_list = []
list_end = len(sys.argv) - 1

for i in range(len(sys.argv)):
   if sys.argv[i] == '-trodenames':
      beg_index = i+1
   elif sys.argv[i][0]=='-':
       list_end  = i
       break

for i in range(beg_index, list_end+1):
   num_list.append(int(sys.argv[i]))
print num_list

dir_list = []
for i in range(len(num_list)):
   if i < 10:
       dir_list.append('tt0' + str(i))
   else:
       dir_list.append('tt' + str(i))
#print dir_list

for i in dir_list:
   if not os.path.exists(i):
       os.makedirs(i)

for i in range(len(num_list)):
    cmd_path = "~/programming/arte-ephys/src/extract/arte_to_tt"
    final_cmd = cmd_path + " -i raw/artedate.data -o ./" + (dir_list[i]) + "/" + (dir_list[i]) + ".tt -trodename " + str(num_list[i])
    spikeparms_cmd = "spikeparms2 ./" +  (dir_list[i]) + "/" + (dir_list[i]) + ".tt -tetrode -binary -parms t_px,t_py,t_pa,t_pb,t_maxwd,t_maxht,time -pos *.p -o " + (dir_list[i]) + "/" + (dir_list[i]) + ".pxyabw"
    os.system(final_cmd)
    os.system(spikeparms_cmd)
