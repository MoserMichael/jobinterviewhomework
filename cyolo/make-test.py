import random

def make_tests(out_file, num_requests, num_words):
    words = [ 'and', 'greetings', 'for', 'receiving', 'Cyolo', 'home', 'assignment','Software', 'Engineer', 'This', 'task', 'examines', 'your', 'back', 'end', 'skills', 'require', 'any', 'prior', 'or', 'domain', 'specific', 'knowledge','Just', 'be', 'yourself', 'and', 'deliver', 'best', 'solution', 'in', 'the', 'allocated', 'time' ]
    
  
    with open(out_file, 'w') as ofile:
        ofile.write("#!/usr/bin/bash\n\n")

        for n in range(0,num_requests):
            line = ""
            for idx in range(num_words):
                if idx > 0:
                    line += ","
                line += words[ random.randint(0, len(words)-1) ]
                
            cmd = f"curl -X POST -H 'Content-Type: application/text' http://localhost:8000/exercise/add-text -d '{line}'\n"
            ofile.write(cmd)
        
        ofile.write("curl http://localhost:8000/exercise/stats\n")  
     


make_tests('run-test.sh', 30, 30)

