import os
import re

DEBUG = 1

def print_requirement():
    print('This script requires two arguments.\n')

    print('In the first argument you choose which algorithm you want to run:')
    print('0: original min cut based algorithm (./multiway1)')
    print('1: alternative (./multiway3)')
    print('2: mixed (./multiwaymix)')
    print('3: sorted (./multiwaysort)')
    print('4: BRKGA with SimpleMultiwaySolver')
    print('5: BRKGA with multiway1 solver\n')

    print('The second argument specify the instances\' directories.\n')

    print('If you want to run Alternative Solver, you also need to specify a third argument:')
    print('0: if you want to run with the original terminal order. 1: if in random order.')
    print('Example of usage: python run_all.py 1 steinb1.txt 0')

# begin functions for human sorting
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
# end functions for human sorting

def save_table_file(which_algorithm, instances_file):

    if which_algorithm == 0:
        sols_file_dir = 'Solutions/Original algorithm/'
        exec_file = 'multiway1'

    if which_algorithm == 1:
        sols_file_dir = 'Solutions/Alternative 2 Shuffle=100/'
        exec_file = 'multiway_alternative_shuffle20'

    if which_algorithm == 2:
        sols_file_dir = 'Solutions/Mixed algorithm/'
        exec_file = 'multiwaymix'
        
    if which_algorithm == 3:
        sols_file_dir = 'Solutions/Sorted algorithm/'
        exec_file = 'multiwaysort'

    if which_algorithm == 4:
        sols_file_dir = 'Solutions/BRKGA simple solver/'
        exec_file = 'simple_multiwaybrkga'

    if which_algorithm == 5:
        sols_file_dir = 'Solutions/BRKGA multiway1 solver perturb2 n=4 with ini pop/'
        exec_file = 'multiway1brkga_perturb2_n4_w_inipop'

    table_file_name = exec_file +'_table'
    table_file_path = 'Solutions/' + table_file_name + ".txt"

    table_file = open(table_file_path, 'w')
    table_file.write('inst num_v num_e num_t custo tempo\n')

    sols_list = os.listdir(sols_file_dir)
    sort_nicely(sols_list)

    for file_name in sols_list:
        if os.path.isdir(file_name):
            continue
        
        instance_path = instances_file + file_name.split('.')[0] + ".txt"
        file_data = open(instance_path, 'r').read()

        lines = file_data.split('\n')
        parts = lines[0].split()
        num_v = int(parts[0])
        num_e = int(parts[1])
        num_t = int(lines[num_e+1])

        file_data = open(sols_file_dir + file_name, 'r').read()
        lines = file_data.split('\n')
        cost = int(lines[0].split()[1])
        exec_time = float(lines[1].split()[1])

        table_file.write("%s %d %d %d %d %.2f\n" %(file_name.split('.')[0], num_v, num_e, num_t, cost, exec_time))

    table_file.close()
    print('Done writing ' + table_file_path + '\n')


def run_instances(exec_file, instances_file, shuffle):
    file_list = os.listdir(instances_file)
    sort_nicely(file_list)

    instance_cnt = 0
    for file_name in file_list:
        if DEBUG >= 1:
            print('Executing ' + exec_file + ' with ' + file_name)

        if shuffle == -1:
            os.system('./' + exec_file + ' '  + file_name)
        else:
            print('./' + exec_file + ' '  + file_name + ' ' + shuffle)
            os.system('./' + exec_file + ' '  + file_name + ' ' + shuffle)

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
    if sys.argv[1] == '1' and len(sys.argv) <= 3:
        print_requirement()
        exit(0)
    
    which_algorithm = int(sys.argv[1].strip())
    print(which_algorithm)
    instances_file = sys.argv[2].strip()
    shuffle = -1

    if which_algorithm == 1:
        shuffle = sys.argv[3]

    if which_algorithm < 0 or which_algorithm > 5:
        print('This algorithm/sols type is not supported.')
    
        exit(0)
    if not os.path.exists(instances_file):
        print('This instance directiory does not exist.')
        exit(0)

    if which_algorithm == 0:
        exec_file = 'run_multiwaysolver 0'
    if which_algorithm == 1:
        exec_file = 'run_multiwaysolver 1'
    if which_algorithm == 2:
        exec_file = 'run_multiwaysolver 2'
    if which_algorithm == 3:
        exec_file = 'run_multiwaysolver 3'
    if which_algorithm == 4:
        exec_file = 'multiwaybrkga 0' # 4: BRKGA with SimpleMultiwaySolver
    if which_algorithm == 5:
        exec_file = 'multiwaybrkga_w_initialpop 1' # 5: BRKGA with multiway1 solver

    #run_instances(exec_file, instances_file, shuffle)
    save_table_file(which_algorithm, instances_file)


if __name__ == "__main__":
    main()
