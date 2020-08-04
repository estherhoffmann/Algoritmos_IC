import os
import re

DEBUG = 1

def print_requirement():
    print('This script requires an argument. Please choose what you want to do:')
    print('\'python run_all.py 0\' to run all instances with ./multiway1')
    print('\'python run_all.py 1\' to run all instances with ./multiway2')
    print('\'python run_all.py 2\' to run all instances with ./multiway3')
    print('\'python run_all.py 3\' to run all instances with the three algorithms')

def tryint(s):
    try:
        return int(s)
    except ValueError:
        return s

# human sorting
def alphanum_key(s):
    # Turn a string into a list of string and number chunks.
        #"z23a" -> ["z", 23, "a"]
    return [ tryint(c) for c in re.split('([0-9]+)', s) ]

def sort_nicely(l):
    #Sort the given list in the way that humans expect.
    l.sort(key=alphanum_key)

def save_table_file(execution_type):
    if execution_type == 0:
        sols_file_dir = os.listdir('Solutions/Original algorithm/')
    if execution_type == 1:
        sols_file_dir = os.listdir('Solutions/Alternative 1/')
    if execution_type == 2:
        sols_file_dir = os.listdir('Solutions/Alternative 2/')


    table_file_name = 'multiway' + str(execution_type+1) +'_table'
    table_file_path = '/Solutions/' + table_file_name + ".sol"

    table_file = open(table_file_path, 'w+')
    table_file.write('inst num_v num_e num_t custo tempo')

    sols_list = os.listdir(sols_file_dir)
    sort_nicely(sols_list)
    print(sols_list)

    '''
    for file_name in sols_list:
        if file_name.endswith(".sol")
            #full_path_graph = "../Multiway Cut/Instances/"+graph_file_name
            sol_file = open(sols_file_dir + '/' + file_name, 'r').read()
            lines = file.split('\n')

            parts = lines[0].split()
            numof_v = int(parts[0])
            numof_e = int(parts[1])

            edges = []
            for i in range(1, numof_e+1):
                parts = lines[i].split()
                edges.append((int(parts[0]), int(parts[1]), int(parts[2])))
    '''
    #f = open(filename)
    #lines = f.read()
    #print (lines[10])


def run_instances(execution_type):
    file_list = os.listdir('Instances')
    sort_nicely(file_list)

    count1 = 0
    '''
    for file_name in file_list:
        if file_name.endswith(".txt"):
            # ./multiway1 < <instances>.txt for all instances
            if execution_type == 0 or execution_type == 3:
                if DEBUG >= 1:
                    print('Executing multiway1 with ' + file_name)
                os.system('./multiway1 '  + file_name)
                count1 += 1
                if DEBUG >= 1:
                    print()
    '''
    if DEBUG >= 1:
        print('Done executing ./multiway1 with ' + str(count1) + ' instances')

def main():
    import sys
    if len(sys.argv) <= 1:
        print_requirement()
        exit(0)

    execution_type = int(sys.argv[1].strip())
    if execution_type > 3:
        print('This execution type is not supported.')
    run_instances(execution_type)

    for i in range(0, execution_type+1):
        save_table_file(i)

if __name__ == "__main__":
    main();
