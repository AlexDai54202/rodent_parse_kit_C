import subprocess

import sys
import os

def main() -> int:
    #print('Number of arguments:', len(sys.argv), 'arguments.')
    #print('Argument List:', str(sys.argv))
    cmd = ['.\\uexp_to_raw.bat']
    failed_to_parse = []
    dirlist = [os.getcwd()]
    while dirlist != []:
        dir = dirlist.pop()
        short_dir = dir.replace(os.getcwd(),'.')
        print("DIRECTORY:", short_dir)
        os.makedirs(f'.\\csv_outputs\\{short_dir[2:]}',exist_ok=True)
        files = os.listdir(dir)
        for f in files:
            dirf = dir +'\\' + f
            if f[-5:] == '.uexp':
                print(str(cmd+[dirf]))
                if (not os.path.isfile(f'.\\csv_outputs\\{short_dir[2:]}\\{f[:-5]}.csv')) or ('\\Event\\' not in dirf): # is Event, has duplicate: don't pass. 
                    p = subprocess.Popen(cmd+[dirf])
                    p.wait()
                    if os.path.isfile('tabular_parsed.csv'):
                        os.replace('.\\tabular_parsed.csv', f'.\\csv_outputs\\{short_dir[2:]}\\{f[:-5]}.csv')
                    else:
                        failed_to_parse += [f'{short_dir[2:]}\\{f[:-5]}']
            if os.path.isdir(dirf):
                print("***"+dirf)
                if 'parse_raw_bytes' not in dirf and 'csv_outputs' not in dirf:
                    dirlist.append(dirf)
    print(failed_to_parse)
    return 0

if __name__ == '__main__':
    sys.exit(main())  # next section explains the use of sys.exit