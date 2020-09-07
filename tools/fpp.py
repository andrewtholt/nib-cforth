#!/usr/bin/env python3
import sys

class forth:
    defs = {}

    fname=""
    idx=0
    flen=0

    def __init__(self,f):
        self.fname = f
        f = open('defs.txt',mode='r')
        while True:
           line = f.readline()
    
           if not line:
               break
           else:
               definition = line.split()
               self.defs[definition[0]] = definition[1]

        self.File = open(self.fname,mode='r')
        self.allFile = self.File.read()

        self.flen=len(self.allFile)
#        print(self.flen)

    def word(self,w):
        out = ''
        if w in self.defs:
            out = self.defs[w]

        return out

    def parse(self,c):

        run=True
        out = ''

        sep = c + '\r\n\t'
        while run:
            if self.allFile[self.idx] in ['\n', '\r', ' ']:
                self.idx = self.idx +1
            else:
                run = False

        out += self.allFile[self.idx]
        self.idx = self.idx +1

        while self.allFile[self.idx] not in sep:
            out += self.allFile[self.idx]
            self.idx = self.idx +1

        self.idx = self.idx +1
        

        return out

defs = {}
def loadDefs():
    global defs

    f = open('defs.txt',mode='r')
    
    while True:
        line = f.readline()

        if not line:
            break
        else:
#            print(line)
            definition = line.split()
            defs[definition[0]] = definition[1]


def main(fname):
    global defs
# Open a file: file
    file = open(fname,mode='r')
 
# read all lines at once
    all_of_it = file.read()

    src = all_of_it.split()

    print(len(src))

    idx = 0;

    while idx < len(src):
        print(src[idx])
        out=''
        if src[idx] == ':':
            out += 'void '
            idx = idx + 1
            out += "h_" + src[idx] + '() {'
            idx = idx + 1
            print(out)
            out = ''
        elif src[idx] == ';':
            out = '}'
            idx = idx + 1
            print(out)
        elif src[idx] == '."':
            if src[idx] in defs:
                print( "    " + defs[src[idx]] + "();")

            idx=idx+2


 
# close the file
    file.close()

if __name__ == "__main__":

    functionList = []

    if len(sys.argv) !=2:
        print("Usage fpp.py <fth file>")
        sys.exit(0)
    else:
#        loadDefs()
#        main(sys.argv[1])

        stringCount = 1
        print("// ")
        print("// Load " + sys.argv[1])
        print("// ")
        p = forth(sys.argv[1])

        while True:
            try:
                tok=p.parse(" ")
            except IndexError:

                print('\nvoid extend_from_file() {')
                for n in functionList:
                    print(n)
                print('}\n')

                sys.exit(0)
    
            out=""
            if tok == ":":
                out += "void "
                name = (p.parse(' ')).rstrip()
                funcName = "f_" + name

                functionList.append( 'create_codeword("' + name + '",' + funcName + ',0);')
    
                out += funcName + "() {"
            elif tok == 's"':
                tmp = (p.parse('"')).rstrip() 
                varName = 'str' + str(stringCount).zfill(3)
                out = '\nconst static char ' + varName + '[] = "' + tmp + '";\n'
                out += 'push(' + varName + ');\n'

                stringCount += 1
                out += 'push(' + str( len(tmp)) + ');\n'


            elif tok == '."':
                out='printf("' + (p.parse('"')).rstrip() + '");'
            elif tok == ';':
                out='}'
            else:
                func = p.word(tok)

                if func != '':
                    out =  func + '();' 
                else:

                    if tok.isnumeric():
                        out = 'push(' + tok + ');'
                    else:
                        out = '// WARNING ' + tok + ' not found'

            print(out)
    




