import pickle as P
import json as J
import glob
import sys
import os

if __name__ == '__main__':
    d = P.load(open('hashdict.pkl', 'rb'))
    errors = dict(
            time_limit=0,
            file_handle=0,
            other=0)

    for hash in d.keys():
        pth = 'outputs/slurm.%s.err' % hash

        if os.path.isfile(pth):
            if os.path.getsize(pth) > 0:
                if 'No such file or directory' in d[hash]['status']['message']:
                    errors['file_handle'] += 1
                    d[hash]['status'] = {
                        'error': True,
                        'message': open(pth).read(),
                        'type': 'file_handle',
                        }
                elif 'due to time limit'.upper() in d[hash]['status']['message']:
                    errors['time_limit'] += 1
                    d[hash]['status'] = {
                        'error': True,
                        'message': open(pth).read(),
                        'type': 'time_limit',
                        }
                else:
                    errors['other'] += 1
                    d[hash]['status'] = {
                        'error': True,
                        'message': open(pth).read(),
                        'type': 'other',
                        }

                continue
        d[hash]['status'] = {
            'error': False,
            'message': None,
            'type': None,
            }

    P.dump(d, open('hashdict.pkl', 'wb'))
    with open('hashdict.json', 'w') as jsondict:
        jsondict.write('%s\n' % J.dumps(d, indent=4))

    sys.stdout.write('{')
    for i, (h, v) in enumerate(d.items()):
        if v['status']['error']:
            sys.stdout.write('\n\t"%s": "%s",' % (h, v['status']['message']))

    sys.stdout.write('\n\t"summary": %s\n}' % J.dumps(errors, indent=4).replace('\n', '\n\t'))

    if len(sys.argv) > 1:
        for i in sys.argv[1:]:
            if i in errors.keys():
                for v in d.values():
                    if v['status']['error']:
                        if v['status']['type'] == i:
                            sys.stdout.write(v['status']['message'])
                            sys.stdout.write('\n')
