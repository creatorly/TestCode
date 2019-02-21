import sys,os,shutil
from fabric.api import *

if len(sys.argv) == 2:
  print "vad:",sys.argv[1]
else:
  print "please enter vad.tgz name"
  exit()

vad = sys.argv[1]
path = "../"

if os.path.exists(vad) == True:
  print path
  shutil.copy(vad, path)
else:
  print "Non-existent"
  exit()

