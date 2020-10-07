# coding=utf-8

# Kamek - build tool for custom C++ code in New Super Mario Bros. Wii
# All rights reserved (c) Treeki 2010 - 2012
# Header files compiled by Treeki, Tempus and megazig

# Requires PyYAML and pyelftools

import os
import os.path
import shutil
import struct
import subprocess
import sys

import elftools.elf.elffile as elf
import hooks
import yaml

version_str = 'Kamek 0.3 by Treeki and RoadRunnerWMC'
u32 = struct.Struct('>I')

verbose = True
use_rels = True
use_mw = False
use_wine = False
mw_path = ''
gcc_path = ''
gcc_type = 'powerpc-eabi'
gcc_append_exe = False
show_cmd = False
delete_temp = True
override_config_file = None
only_build = []
fast_hack = False
current_unique_id = 0
cext = ['.cpp', '.c', '.cc', '.cxx', 'c++']


def parse_cmd_options():
    global use_rels, use_mw, use_wine, show_cmd, delete_temp, only_build, fast_hack
    global override_config_file, gcc_type, gcc_path, gcc_append_exe, mw_path

    if '--no-rels' in sys.argv:
        use_rels = False

    if '--use-mw' in sys.argv:
        use_mw = True

    if '--use-wine' in sys.argv:
        use_wine = True

    if '--show-cmd' in sys.argv:
        show_cmd = True

    if '--keep-temp' in sys.argv:
        delete_temp = False

    if '--fast-hack' in sys.argv:
        fast_hack = True

    if '--gcc-append-exe' in sys.argv:
        gcc_append_exe = True

    for arg in sys.argv:
        if arg.startswith('--gcc-path='):
            gcc_path = arg[11:] + '/'

        elif arg.startswith('--mw-path='):
            mw_path = arg[10:] + '/'

        elif arg.startswith('--configs='):
            override_config_file = arg[10:]

        elif arg.startswith('--build='):
            only_build.append(arg[8:])

        elif arg.startswith('--gcc-type='):
            gcc_type = arg[11:]


def print_debug(s):
    if verbose:
        print('*', str(s))


def read_configs(filename):
    with open(filename) as f:
        data = f.read()
        return yaml.safe_load(data)


def generate_unique_id():
    """
    This is used for temporary filenames, to ensure that .o files do not overwrite each other
    """
    global current_unique_id
    current_unique_id += 1
    return current_unique_id


def align_addr_up(addr, align):
    align -= 1
    return (addr + align) & ~align


def generate_kamek_patches(patchlist):
    kamekpatch = b''

    for patch in patchlist:
        if len(patch[1]) > 4:
            # Create patch
            kamekpatch += u32.pack(align_addr_up(len(patch[1]), 4) // 4)
            kamekpatch += u32.pack(patch[0])
            kamekpatch += patch[1]

            # Align it by 4
            r = len(patch[1]) % 4
            if r:
                kamekpatch += b'\0' * (4 - r)
        else:
            # Single patch
            kamekpatch += u32.pack(patch[0])
            kamekpatch += patch[1]

    kamekpatch += u32.pack(0xFFFFFFFF)
    return kamekpatch


class DyLinkCreator:
    R_PPC_ADDR32 = 1
    R_PPC_ADDR16_LO = 4
    R_PPC_ADDR16_HI = 5
    R_PPC_ADDR16_HA = 6
    R_PPC_REL24 = 10

    VALID_RELOCS = {R_PPC_ADDR32, R_PPC_ADDR16_LO, R_PPC_ADDR16_HI, R_PPC_ADDR16_HA, R_PPC_REL24}

    def __init__(self, other=None):
        if other:
            self._relocs = other._relocs[:]

            self._targets = other._targets[:]
            self._target_lookups = other._target_lookups.copy()
        else:
            self._relocs = []

            self._targets = []
            self._target_lookups = {}

        self.elf = None

    def set_elf(self, stream):
        if self.elf:
            raise ValueError('ELF already set')

        self.elf = elf.ELFFile(stream)
        self.code = self.elf.get_section_by_name('.text').data()

        self._add_relocs(self.elf.get_section_by_name('.rela.text'))

    def _add_relocs(self, section):
        sym_values = {}
        sym_section = self.elf.get_section_by_name('.symtab')
        
        if section:
            for reloc in section.iter_relocations():
                entry = reloc.entry
                sym_id = entry['r_info_sym']

                # Check if symbol already exists, otherwise add it to the dict
                try:
                    sym_value, sym_name = sym_values[sym_id]
                except KeyError:
                    sym = sym_section.get_symbol(sym_id)
                    sym_value = sym.entry['st_value']
                    sym_name = sym.name
                    sym_values[sym_id] = (sym_value, sym_name)

                self.add_reloc(entry['r_info_type'], entry['r_offset'], sym_value + entry['r_addend'], sym_name)

    def add_reloc(self, reltype, addr, target, name="UNKNOWN NAME"):
        if reltype not in self.VALID_RELOCS:
            raise ValueError('Unknown/unsupported rel type: %d (%x => %x)' % (reltype, addr, target))

        try:
            target_id = self._target_lookups[target]
        except KeyError:
            target_id = len(self._targets)
            self._target_lookups[target] = target_id
            self._targets.append(target)

        if target <= 0 and name != 'UNKNOWN NAME':
            print("Warning: The following reloc (%x) points to %d: Is this right? %s" % (addr, target, name))

        self._relocs.append((reltype, addr, target_id))

    def build_reloc_data(self):
        header_struct = struct.Struct('>8sI')
        rel_struct_pack = struct.Struct('>II').pack
        target_struct_pack = struct.Struct('>I').pack

        rel_data = map(lambda x: rel_struct_pack((x[0] << 24) | x[2], x[1]), self._relocs)
        target_data = map(target_struct_pack, self._targets)
        header = header_struct.pack(b'NewerREL', 12 + (len(self._relocs) * 8))

        return b''.join([header, b''.join(rel_data), b''.join(target_data)])


class KamekModule:
    _requiredFields = ['source_files']

    def __init__(self, filename):
        # Load the module data
        self.modulePath = os.path.normpath(filename)
        self.moduleName = os.path.basename(self.modulePath)
        self.moduleDir = 'processed'

        with open(self.modulePath) as f:
            rawdata = f.read()
            self.data = yaml.safe_load(rawdata)

        # Verify it
        if not isinstance(self.data, dict):
            raise ValueError('The module file %s has an invalid format (it should be a YAML mapping)' % self.moduleName)

        for field in self._requiredFields:
            if field not in self.data:
                raise ValueError('Missing field in the module file %s: %s' % (self.moduleName, field))


class KamekBuilder:
    def __init__(self, project, configs):
        self.project = project
        self.configs = configs

    def build(self):
        print_debug('Starting build')

        # Create NewerASM folder
        self._prepare_dirs()

        for config in self.configs:

            # Ignore this region if it's not defined in the build command
            if only_build and config['short_name'] not in only_build:
                continue

            # Read the config file
            self._set_config(config)

            # Create temp dir
            self._configTempDir = 'tmp'
            if os.path.isdir(self._configTempDir):
                shutil.rmtree(self._configTempDir)
            os.mkdir(self._configTempDir)
            print_debug('Temp files for this configuration are in: ' + self._configTempDir)

            # Set the DyLinkCreator if dynamic linking is enabled
            if 'dynamic_link' in self._config and self._config['dynamic_link']:
                self.dynamic_link_base = DyLinkCreator()
            else:
                self.dynamic_link_base = None

            # Set the code address
            self._builtCodeAddr = 0x80001800
            if 'code_address' in self.project.data:
                self._builtCodeAddr = self.project.data['code_address']

            # Compile the modules
            self._compile_modules()

            # Figure out how many copies we need to build
            if 'multi_build' in self._config:
                self._multi_build = self._config['multi_build']
            else:
                self._multi_build = {self._config['short_name']: self._config['linker_script']}

            for s_name, s_script in self._multi_build.items():
                self.current_build_name = s_name

                # Rename the file to the final scheme, it will be used later
                if 'pal' in s_name:
                    s_name = s_name.replace('pal', 'EU_')
                elif 'ntsc' in s_name:
                    s_name = s_name.replace('ntsc', 'US_')
                elif 'jpn' in s_name:
                    s_name = s_name.replace('jpn', 'JP_')
                elif 'kor' in s_name:
                    s_name = s_name.replace('kor', 'KR_')
                if '2' not in s_name:
                    s_name += '1'

                self._patches = []
                self._rel_patches = []
                self._hooks = []

                # Create hooks
                self._create_hooks()

                # Link the code
                self._link(self.current_build_name, 'tools/' + s_script)
                self._read_symbol_map()

                if self.dynamic_link_base:
                    self.dynamic_link = DyLinkCreator(self.dynamic_link_base)
                    with open(self._currentOutFile, 'rb') as f:
                        self.dynamic_link.set_elf(f)

                for hook in self._hooks:
                    hook.create_patches()

                self._create_patch(s_name)

            if delete_temp:
                shutil.rmtree(self._configTempDir)

    def _prepare_dirs(self):
        self._outDir = self.project.makeRelativePath(self.project.data['output_dir'])
        print_debug('Project will be built in: ' + self._outDir)

        if not os.path.isdir(self._outDir):
            os.mkdir(self._outDir)
            print_debug('Created that directory')

    def _set_config(self, config):
        self._config = config
        print_debug('---')
        print_debug('Building for configuration: ' + config['friendly_name'])

        self.config_short_name = config['short_name']
        if 'rel_area_start' in config:
            self._rel_area = (config['rel_area_start'], config['rel_area_end'])
        else:
            self._rel_area = (-50, -50)

    def _compile_modules(self):
        print_debug('---')
        print_debug('Compiling modules')

        # Setup the compiler
        if use_mw:
            cc_command = ['%smwcceppc.exe' % mw_path, '-I.', '-I-', '-I.', '-nostdinc', '-Cpp_exceptions', 'off', '-Os', '-proc', 'gekko', '-fp', 'hard', '-enum', 'int', '-sdata', '0', '-sdata2', '0', '-g', '-RTTI', 'off', '-use_lmw_stmw', 'on']
            as_command = ['%smwasmeppc.exe' % mw_path, '-I.', '-I-', '-I.', '-nostdinc', '-proc', 'gekko', '-d', '__MWERKS__']

            if 'defines' in self._config:
                for d in self._config['defines']:
                    cc_command.append('-d')
                    cc_command.append(d)
                    as_command.append('-d')
                    as_command.append(d)

            for i in self._config['include_dirs']:
                cc_command.append('-I%s' % i)
                as_command.append('-I%s' % i)

            if use_wine:
                cc_command.insert(0, 'wine')
                as_command.insert(0, 'wine')

        else:
            cc_command = as_command = ['%s%s-g++' % (gcc_path, gcc_type), '-nodefaultlibs', '-I.', '-fno-builtin', '-Os', '-fno-exceptions', '-fno-rtti', '-mno-sdata']

            if 'defines' in self._config:
                for d in self._config['defines']:
                    cc_command.append('-D%s' % d)

            for i in self._config['include_dirs']:
                cc_command.append('-I%s' % i)

        self._moduleFiles = []

        # Setup fast hack if enabled
        if fast_hack:
            fast_cpp_path = os.path.join(self._configTempDir, 'fasthack.cpp')
            fast_cpp = open(fast_cpp_path, 'w')

        # Process each file
        for m in self.project.modules:
            for normal_sourcefile in m.data['source_files']:
                print_debug('Compiling %s : %s' % (m.moduleName, normal_sourcefile))

                objfile = os.path.join(self._configTempDir, '%d.o' % generate_unique_id())
                sourcefile = os.path.join(m.moduleDir, normal_sourcefile)

                # It's an object
                if sourcefile.endswith('.o'):
                    new_command = ['cp', sourcefile, objfile]
                else:
                    # It's ASM
                    if os.path.splitext(sourcefile)[1].lower() == '.s':
                        new_command = as_command

                    # It's C/C++
                    elif os.path.splitext(sourcefile)[1].lower() in cext and fast_hack:
                        fast_cpp.write('//\n// %s\n//\n\n' % sourcefile)

                        with open(sourcefile) as sf:
                            fast_cpp.write(sf.read())

                        fast_cpp.write('\n')
                        continue
                    else:
                        new_command = cc_command

                    new_command += ['-c', '-o', objfile, sourcefile]

                    if 'cc_args' in m.data:
                        new_command += m.data['cc_args']

                # Show command if enabled
                if show_cmd:
                    print_debug(new_command)

                # Run the compiler!
                errorVal = subprocess.call(new_command)
                if errorVal != 0:
                    print('BUILD FAILED!')
                    print('Compiler returned %d - an error occurred while compiling %s' % (errorVal, sourcefile))
                    sys.exit(1)

                # Add the module to the list of compiled objects
                self._moduleFiles.append(objfile)

        # If the fast hack is enabled, compile it
        if fast_hack:
            fast_cpp.close()

            print_debug('Compiling the fast hack...')
            objfile = os.path.join(self._configTempDir, 'fasthack.o')

            # Build the command and print it if the option is enabled
            new_command = cc_command + ['-c', '-o', objfile, fast_cpp_path]
            if show_cmd:
                print_debug(new_command)

            # Run the compiler!
            errorVal = subprocess.call(new_command)
            if errorVal != 0:
                print('BUILD FAILED!')
                print('Compiler returned %d - an error occurred while compiling the fast hack' % errorVal)
                sys.exit(1)

            # Add the module to the list of compiled objects
            self._moduleFiles.append(objfile)
        print_debug('Compilation complete')

    def _create_hooks(self):
        print_debug('---')
        print_debug('Creating hooks')

        for m in self.project.modules:
            if 'hooks' in m.data:
                for hookData in m.data['hooks']:
                    # Check that the hook has a name and a type
                    if not('name' in hookData and 'type' in hookData):
                        raise KeyError('Invalid hook')

                    # Now validate it
                    if hookData['type'] in hooks.HookTypes:
                        hookType = hooks.HookTypes[hookData['type']]
                        self._hooks.append(hookType(self, m, hookData))
                    else:
                        raise ValueError('Unknown hook type: %s' % hookData['type'])

    def _link(self, short_name, script_file):
        print_debug('---')
        print_debug('Linking %s (%s)...' % (short_name, script_file))
        print_debug('---')

        # Locate map and plf files
        self._currentMapFile = '%s/%s_linkmap.map' % (self._outDir, short_name)
        outname = 'object.plf' if self.dynamic_link_base else 'object.bin'
        self._currentOutFile = '%s/%s_%s' % (self._outDir, short_name, outname)

        # Build the command
        ld_command = ['%s%s-ld%s' % (gcc_path, gcc_type, '.exe' if gcc_append_exe else ''), '-L.', '-o', self._currentOutFile,
                      '-T', script_file, '-Map', self._currentMapFile, '--no-demangle']
        if self.dynamic_link_base:
            ld_command.append('-r')
            ld_command.append('--oformat=elf32-powerpc')
        else:
            ld_command.append('--oformat=binary')
            ld_command.append('-Ttext')
            ld_command.append('0x%08X' % self._builtCodeAddr)
        ld_command += self._moduleFiles

        # Print the command if the option is enabled
        if show_cmd:
            print_debug(ld_command)

        # Call the linker!
        errorVal = subprocess.call(ld_command)
        if errorVal != 0:
            print('BUILD FAILED!')
            print('ld returned %d' % errorVal)
            sys.exit(1)

        print_debug('Successfully linked %s' % short_name)

    def _read_symbol_map(self):
        print_debug('---')
        print_debug('Reading symbol map')

        self._symbols = []

        with open(self._currentMapFile) as file:
            for line in file:
                if '__text_start' in line:
                    self._textSegStart = int(line.split()[0], 16)
                    break

            # Now read the individual symbols
            for line in file:
                if '__text_end' in line:
                    self._textSegEnd = int(line.split()[0], 16)
                    break

                if not line.startswith(' ' * 16):
                    continue

                sym = line.split()
                sym[0] = int(sym[0], 16)
                self._symbols.append(sym)

            # We've found __text_end, so now we should be at the output section
            currentEndAddress = self._textSegEnd

            for line in file:
                # Look for segments
                if line[0] == '.':
                    data = line.split()
                    if len(data) < 3:
                        continue

                    segAddr = int(data[1], 16)
                    segSize = int(data[2], 16)

                    if segAddr + segSize > currentEndAddress:
                        currentEndAddress = segAddr + segSize

            self._codeStart = self._textSegStart
            self._codeEnd = currentEndAddress

        print_debug('Read, %d symbol(s) parsed.' % len(self._symbols))

        # Next up, run it through c++filt
        print_debug('Running c++filt...')
        p = subprocess.Popen('%s/powerpc-eabi-c++filt.exe' % gcc_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

        symbolNameList = list(zip(*self._symbols))[1]
        filtResult = p.communicate('\n'.join(symbolNameList).encode('utf-8', 'ignore'))
        filteredSymbols = filtResult[0].decode('utf-8', 'ignore').splitlines()

        for sym, filt in zip(self._symbols, filteredSymbols):
            sym[1] = filt.strip()

        print_debug('Done. All symbols complete.')
        print_debug('Generated code is at 0x%08X .. 0x%08X' % (self._codeStart, self._codeEnd - 4))

    def find_func_by_symbol(self, find_symbol):
        for sym in self._symbols:
            if sym[1] == find_symbol:
                return sym[0]

        raise ValueError('Cannot find function: %s' % find_symbol)

    def add_patch(self, offset, data):
        if self._rel_area[0] <= offset <= self._rel_area[1] and use_rels:
            self._rel_patches.append((offset, data))
        else:
            self._patches.append((offset, data))

    def _create_patch(self, short_name):
        print_debug('---')
        print_debug('Creating patch')

        # Convert the .rel patches to KamekPatcher format
        if self._rel_patches:
            kamekpatch = generate_kamek_patches(self._rel_patches)
            self._patches.append((0x80002F60, kamekpatch))

        if self.dynamic_link:
            # Put together the dynamic link files
            with open('%s/DLCode%s.bin' % (self._outDir, short_name), 'wb') as dlcode:
                dlcode.write(self.dynamic_link.code)

            with open('%s/DLRelocs%s.bin' % (self._outDir, short_name), 'wb') as dlrelocs:
                dlrelocs.write(self.dynamic_link.build_reloc_data())

        else:
            # Add the outfile as a patch if not using dynamic linking
            with open(self._currentOutFile, 'rb') as file:
                patch = (self._codeStart, file.read())

            self._patches.append(patch)

        # Generate a KamekPatcher patch
        with open('%s/System%s.bin' % (self._outDir, short_name), 'wb') as kpatch:
            kpatch.write(generate_kamek_patches(self._patches))

        print_debug('Patches generated')


class KamekProject(object):
    _requiredFields = ['output_dir', 'modules']

    def __init__(self, filename):
        # Load the project data
        self.configs = None
        self.projectPath = os.path.abspath(filename)
        self.projectDir = ''

        with open(self.projectPath, 'r') as f:
            rawdata = f.read()
            self.data = yaml.safe_load(rawdata)

        # Verify it
        if not isinstance(self.data, dict):
            raise ValueError('The project file has an invalid format (it should be a YAML mapping)')

        for field in self._requiredFields:
            if field not in self.data:
                raise ValueError('Missing field in the project file: %s' % field)

        # Load each module
        self.modules = []
        if self.data['modules']:
            for moduleName in self.data['modules']:
                modulePath = self.makeRelativePath(moduleName)
                self.modules.append(KamekModule(modulePath))

    def makeRelativePath(self, path):
        return os.path.normpath(os.path.join(self.projectDir, path))

    def build(self):
        # Compile everything in the project
        KamekBuilder(self, self.configs).build()


def main():
    print(version_str)
    print('')

    if len(sys.argv) < 2:
        print('No input file specified')
        sys.exit(1)

    parse_cmd_options()

    project = KamekProject(os.path.normpath(sys.argv[1]))

    if override_config_file:
        project.configs = read_configs(override_config_file)
    else:
        project.configs = read_configs('tools/kamek_configs.yaml')

    project.build()


if __name__ == '__main__':
    main()
