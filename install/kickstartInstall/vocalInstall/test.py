import getopt
args = [ '-a', '-b', 'foo', '--exclude', 'bar', 'x1','x2']
opts, pargs = getopt.getopt()
print `opts`
print `pargs`
