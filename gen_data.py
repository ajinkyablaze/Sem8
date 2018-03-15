import re

index_file = open("conflation_input/index.txt","w")

def func(x):
    f = open("wikipedia-first.txt")
    file_data = f.readlines()
    for k in range(0,3*x,3):
        clean_line = re.sub(' ','_',file_data[k].strip())
        data = file_data[k+1].strip()
        f_data = open("conflation_input/" + clean_line + ".txt","w")
        f_data.write(data)
        f_data.close()
    f.close()    

func(3)
index_file.close();
    
