import os
import re

DEBUG = 1

def print_requirement():
    print('This script requires two arguments.\n')
    print('The first argument specify what you want to do:')
    print('0: just run the algorithm\n1: just build the table file\n2: both.\n')
    print('The second argument specify which algorithm/sols you want to use:')
    print('0: original min cut based algorithm\n1: alternative (./multiway3)\n2: mixed 2 (./multiwaymix)\n3: BRKGA with SimpleMultiwaySolver \n4:all of them')

# functions for human sorting
def tryint(s):
    try:
        return int(s)
    except ValueError:
        return s

def alphanum_key(s):
    # Turn a string into a list of string and number chunks.
        #"z23a" -> ["z", 23, "a"]
    return [ tryint(c) for c in re.split('([0-9]+)', s) ]

def sort_nicely(l):
    #Sort the given list in the way that humans expect.
    l.sort(key=alphanum_key)
# end of functions for human sorting

def save_table_file(current_execution, exec_file):
    if current_execution == 0:
        sols_file_dir = 'Solutions/Original algorithm/'
    if current_execution == 1:
        sols_file_dir = 'Solutions/Alternative 2/'
    if current_execution == 2:
        sols_file_dir = 'Solutions/Mixed algorithm/'
    if current_execution == 3:
        sols_file_dir = 'Solutions/BRKGA simple solver/'

    table_file_name = exec_file +'_table'
    table_file_path = 'Solutions/' + table_file_name + ".txt"

    table_file = open(table_file_path, 'w')
    table_file.write('inst num_v num_e num_t custo tempo\n')

    sols_list = os.listdir(sols_file_dir)
    sort_nicely(sols_list)

    for file_name in sols_list:
        instance_path = "Instances/" + file_name.split('.')[0] + ".txt"
        file_data = open(instance_path, 'r').read()

        lines = file_data.split('\n')
        parts = lines[0].split()
        num_v = int(parts[0])
        num_e = int(parts[1])
        num_t = int(lines[num_e+1])

        file_data = open(sols_file_dir + file_name, 'r').read()
        lines = file_data.split('\n')
        #parts = lines[0].split()
        cost = int(lines[0].split()[1])
        #parts = lines[1].split()
        exec_time = float(lines[1].split()[1])

        table_file.write("%s %d %d %d %d %.2f\n" %(file_name.split('.')[0], num_v, num_e, num_t, cost, exec_time))

    table_file.close()
    print('Done writing ' + table_file_path + '\n')

def run_instances(exec_file):
    file_list = os.listdir('Instances/')
    sort_nicely(file_list)

    instance_cnt = 0
    for file_name in file_list:
        if DEBUG >= 1:
            print('Executing ' + exec_file + ' with ' + file_name)
        os.system('./' + exec_file + " "  + file_name)
        instance_cnt += 1
        if DEBUG >= 1:
            print()

    if DEBUG >= 1:
        print('Done executing ./' + exec_file +  ' with ' + str(instance_cnt) + ' instances\n')

def main():
    import sys
    if len(sys.argv) <= 2:
        print_requirement()
        exit(0)

    execution_type = int(sys.argv[1].strip())
    which_algorithm = int(sys.argv[2].strip())
    if execution_type > 2 or execution_type < 0:
        print('This execution type is not supported.')
        exit(0)
    if which_algorithm > 4 or which_algorithm < 0:
        print('This algorithm/sols type is not supported.')
        exit(0)

    if which_algorithm == 1:
        exec_file = 'multiway3'
    if which_algorithm == 2:
        exec_file = 'multiwaymix'
    if which_algorithm == 3:
        exec_file = 'alg_simple_multiwaybrkga'

    # if the code will use only one algorithm (or its solution)
    if which_algorithm >= 0 and which_algorithm < 4:
        if execution_type == 0 or execution_type == 2: # if we want to run instances
            run_instances(exec_file)
        if execution_type == 1 or execution_type == 2: # if we want to save the table file
            save_table_file(which_algorithm, exec_file)

    # if we will use all algorithms
    if which_algorithm == 4:
        for i in range(0, which_algorithm):
            if execution_type == 0 or execution_type == 2: # if we want to run instances
                run_instances(i)
            if execution_type == 1 or execution_type == 2: # if we want to save the table file
                save_table_file(i)

if __name__ == "__main__":
    main()
