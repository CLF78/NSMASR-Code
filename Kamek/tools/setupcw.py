import os

lmgr8 = 'lmgr8c.dll'
lmgr11 = 'lmgr11.dll'
ogpattern = b'\x89\x45\xFC\x83\x7D\xFC\x00\x75\x0C'
newpattern = b'\xC7\x45\xFC\0\0\0\0\xEB\x0C'

print('CodeWarrior Unlocker by CLF78, patch by Skogkatt\n')
print('NOTE: You only need to run this once!\n')
print('Please insert the CodeWarrior path. It must have the following files in it:')
print('-', lmgr8)
print('-', lmgr11)

while True:
    cwpath = input('Insert CodeWarrior path: ')
    if os.path.isfile(os.path.join(cwpath, lmgr8)):
        lmgr8 = os.path.join(cwpath, lmgr8)
        lmgr11 = os.path.join(cwpath, lmgr11)
        break
    print('Could not find DLL in this folder. Please try again')

if os.path.isfile(lmgr11):
    print('Removing lmgr11.dll...')
    os.remove(lmgr11)

print('Patching lmgr8.dll...')
with open(lmgr8, 'rb+') as dll:
    data = dll.read()
    if newpattern in data:
        print('File already patched. Skipping...')
    else:
        data.replace(ogpattern, newpattern)
        dll.write(data)
        print('File patched!')

print('Renaming file...')
os.rename(lmgr8, lmgr11)
print('All done!')
