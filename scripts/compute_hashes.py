#! /usr/bin/env python

import glob
import os
import re
import subprocess

hashes = glob.glob('sbatch-*.sh')

hashes = [ re.sub('sbatch-(.*)\.sh', r'\1', h) for h in hashes ]

subprocess.check_output(['ls', '--full-time', ''])
print(hashes[:5])
