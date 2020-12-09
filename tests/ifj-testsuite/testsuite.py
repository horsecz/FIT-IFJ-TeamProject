# Testing framework for IFJ 2020 projects

"""
USER TESTING SCRIPT - RUN BUT DO NOT EDIT
"""

# Imports
import argparse
import os
import json
import subprocess
import signal
import shutil
import re

# Default argument values
DEFAULT_COMPILER_PATH = './ifj20'
DEFAULT_LOG_FILE = './log'
DEFAULT_TIMEOUT = 5
DEFAULT_OUTPUT_FOLDER = './outputs'
DEFAULT_TEST_DIR = './tests'
DEFAULT_GO_INTERPRETER = 'go'
DEFAULT_IFJCODE_INTERPRETER = './ic20int'
DEFAULT_GO_INCLUDE = './ifj20.go'
DEFAULT_TMP_DIR = './tmp'

# Tmp file names
TMP_TEMPLATE_FILE_NAME = 'ifj20.go'
TMP_GO_FILE_NAME = 'in.go'
TMP_IFJCODE_FILE_NAME = 'out.ifjcode'

# List of extensions
EXTENSIONS = ['BOOLTHEN', 'BASE', 'FUNEXP', 'MULTIVAL', 'UNARY']

# Global variables
test_index = 0
test_id = ""
log = None
logEnable = True
process = None

def Log (message):
    if logEnable:
        if log is None:
            print(message)
        else:
            log.write(message + '\n')

# Function to parse command line arguments
def ParseArgs ():
    # Define parser
    parser = argparse.ArgumentParser(description="run tests for IFJ20 project")

    # Define arguments for mode selection
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--mode-compile-only', '-mc', action='store_true', help='compile only mode runs only compiler for each test (no interpretation and no output checking)')
    group.add_argument('--mode-interpret-only', '-mi', action='store_true', help='interpret only mode runs only compiler and interpreter for each test (no output checking)')
    group.add_argument('--mode-all', '-ma', action='store_true', help='all mode runs compiler and interpreter for each test and then checks outputs with native go language interpreter. This option is default')

    # Define arguments for test selection
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--select', '-s', default=DEFAULT_TEST_DIR, help='single test file or test directory that should be run (relative to main tests folder). example: "lex/symbols". default: All tests in main tests directory are run')
    group.add_argument('--select-file', '-sf', help="path to a file that specifies multiple test files or tests directories to be run (each on separate line).")

    # Define commonly used arguments
    parser.add_argument('--compiler', '-c', default=DEFAULT_COMPILER_PATH, help='path to the IFJ20 language compiler (the IFJ project executable). default: ' + DEFAULT_COMPILER_PATH)
    parser.add_argument('--extensions', '-e', default='', help='list of implemented extensions. example: "BOOLTHEN,BASE". default: No extension implemented. options: ' + ', '.join(EXTENSIONS))
    group = parser.add_mutually_exclusive_group()   
    group.add_argument('--log-file', '-lf', default=DEFAULT_LOG_FILE, help='path to the log file created by the testsuite (if file already exists, it will be deleted). default: ' + DEFAULT_LOG_FILE)
    group.add_argument('--log-output', '-lo', action='store_true', help='print logs to the standard output instead of a file')
    group.add_argument('--log-none', '-ln', action='store_true', help='do not print logs to output or a file')
    parser.add_argument('--log-success-output', '-ls', action='store_true', help='log interpret output from successfull tests')
    parser.add_argument('--save-ifjcode-all', '-a', action='store_true', help='stores compiler results (ifjcode files) for all tests (not only failed ones)')
    parser.add_argument('--timeout', '-t', default=DEFAULT_TIMEOUT, type=int, help='specify maximum timeout for each test in seconds (required to detect infinite run errors). defult = ' + str(DEFAULT_TIMEOUT))
    parser.add_argument('--output-folder', '-o', default=DEFAULT_OUTPUT_FOLDER, help='path to the folder where compiler output (IFJ20code language programs) is stored for every test that fails on interpretation or checking (if folder already exists, it will be deleted). default: ' + DEFAULT_OUTPUT_FOLDER)

    # Define other arguments
    parser.add_argument('--go-interpreter', default=DEFAULT_GO_INTERPRETER, help='command to execute native go interpreter for output checking. default: ' + DEFAULT_GO_INTERPRETER)
    parser.add_argument('--ifjcode-interpreter', default=DEFAULT_IFJCODE_INTERPRETER, help='command to execute IFJ20code interpreter for compiler output interpretation. default: ' + DEFAULT_IFJCODE_INTERPRETER)
    parser.add_argument('--go-include-file', default=DEFAULT_GO_INCLUDE, help='path to the file that is required to be included in go programs to execute ifj language. default: ' + DEFAULT_GO_INCLUDE)
    parser.add_argument('--tmp-dir', default=DEFAULT_TMP_DIR, help='path to a temp directory that will be created to store temp files for tests. default: ' + DEFAULT_TMP_DIR)

    # Parse arguments from command line
    args = parser.parse_args()

    # Argument postprrocessing
    if not args.mode_compile_only and not args.mode_interpret_only and not args.mode_all:
        print('setting default mode --mode-all')
        args.mode_all = True

    if args.select_file is not None:
        if not os.path.isfile(args.select_file):
            raise Exception('Test selection file\'' + args.select_file + '\' does not exists')
        print('parsing test selection file')
        with open(args.select_file) as f:
            while True:
                line = f.readline()
                if line == '':
                    break
                line = line.strip()
                if not os.path.isfile(line) and not os.path.isdir(line):
                    raise Exception('\'' + line + '\' line in test selection file is not a valid test file or test directory')
                args.select.append(line)
    else:
        if not os.path.isfile(args.select) and not os.path.isdir(args.select):
            raise Exception('Test selection \'' + args.select + '\'  is not a valid test file or test directory')
        args.select = [args.select]

    if not os.path.isfile(args.compiler):
        raise Exception('The path \'' + args.compiler + '\' is not a valid compiler executable')

    if args.extensions != '':
        print('parsing extensions')
        args.extensions = args.extensions.split(',')
        for ext in args.extensions:
            if ext not in EXTENSIONS:
                raise Exception('Invalid extension \'' + ext + '\'')
    else:
        args.extensions = []

    if args.log_output or args.log_none:
        pass
    elif os.path.isfile(args.log_file):
        print('removing old log file \'' + args.log_file + '\'')
        os.remove(args.log_file)
    if os.path.isdir(args.log_file):
        raise Exception('There is a directory with the same name as specified log file \'' + args.log_file + '\'')

    if args.timeout <= 0:
        raise Exception('Value of timeout must be greater then zero, but is \'' + str(args.timeout) + '\'')

    if os.path.isfile(args.output_folder):
        raise Exception('There is a file with the same name as specified output folder \'' + args.output_folder + '\'')
    if os.path.isdir(args.output_folder):
        print('removing old output folder \'' + args.output_folder + '\'')
        shutil.rmtree(args.output_folder)
    print('creating output folder \'' + args.output_folder + '\'')
    os.mkdir(args.output_folder)

    print('checking go interpreter')
    try:
        output = subprocess.check_output([args.go_interpreter, 'version'])
    except Exception as ex:
        raise Exception('Go interpreter is not valid. Command: \'' + args.go_interpreter + ' version\' couldn\'t be executed. Reason: ' + str(ex))
    if output[:11] != 'go version ':
        raise Exception('Go interpreter is not valid. Command: \'' + args.go_interpreter + ' version\' didn\'t produce correct output')
    print('go interpreter found in version \'' + output[11:-1] + '\'')

    print('checking ifjcode interpreter')
    try:
        output = subprocess.check_output([args.ifjcode_interpreter, '--help'])
    except Exception as ex:
        raise Exception('Ifjcode interpreter is not valid. Command: \'' + args.ifjcode_interpreter + ' --help\' couldn\'t be executed. Reason:' + str(ex))
    if output[:7] != 'BUILD: ':
        raise Exception('Ifjcode interpreter is not valid. Command: \'' + args.ifjcode_interpreter + ' --help\' didn\'t produce correct output')
    print('ifjcode interpreter found')

    if not os.path.isfile(args.go_include_file):
        raise Exception('The path \'' + args.go_include_file + '\' is not a valid go include file')

    if os.path.isfile(args.tmp_dir):
        raise Exception('There is a file with the same name as specified tmp directory \'' + args.tmp_dir + '\'')
    if os.path.isdir(args.tmp_dir):
        print('using exesting tmp directory  \'' + args.tmp_dir + '\'')
    else:
        print('creating tmp directory  \'' + args.tmp_dir + '\'')
        os.mkdir(args.tmp_dir)
    shutil.copyfile(args.go_include_file, os.path.join(args.tmp_dir, TMP_TEMPLATE_FILE_NAME))

    return args

def ProcessTests(args):
    def ProcessTestFile(path):
        result = {}
        result['name'] = path
        result['nogo'] = False
        result['code'] = ''
        result['compiler'] = []
        result['interpret'] = []
        result['extensions+'] = []
        result['extensions-'] = []
        result['scenarios'] = []
        with open(path, 'r') as f:
            while True:
                line = f.readline().strip()
                if line.startswith('//compiler '):
                    if result['compiler'] != []:
                        raise Exception('compiler pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['compiler'].append(int(item))
                elif line.startswith('//nogo'):
                    if result['nogo']:
                        raise Exception('nogo pragma present multiple times in test header of test file \'' + path + '\'')
                    result['nogo'] = True
                elif line.startswith('//interpret '):
                    if result['interpret'] != []:
                        raise Exception('interpret pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['interpret'].append(int(item))
                elif line.startswith('//extensions+ '):
                    if result['extensions+'] != []:
                        raise Exception('extensions+ pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['extensions+'].append(int(item))
                elif line.startswith('//extensions- '):
                    if result['extensions-'] != []:
                        raise Exception('extensions- pragma present multiple times in test header of test file \'' + path + '\'')
                    for item in line.split()[1:]:
                        result['extensions-'].append(int(item))
                elif line.startswith('//input '):
                    scenario = line.split()
                    if (len(scenario) != 2):
                        raise Exception('input pragma does not contain a file')
                    inputFile = os.path.join(os.path.dirname(path), scenario[1])
                    if not os.path.isfile(inputFile):
                        raise Exception('input file \'' + inputFile + '\' in scenario test \'' + path + '\' is not valid file')
                    with open(inputFile, 'r') as inputF:
                        content = inputF.read()
                    result['scenarios'].append((scenario[1], content))
                elif line == '//':
                    content = ''
                    while True:
                        line = f.readline()
                        if line == '':
                            break
                        content += line
                    result['code'] = content
                    break
                elif line.startswith('// '):
                    continue
                else:
                    raise Exception('invalid test header in test file \'' + path + '\'')

        for ext in result['extensions+']:
            if ext not in EXTENSIONS:
                raise Exception('Unrecognized extension \'' + ext + '\' in test header of test file \'' + path + '\'')
        for ext in result['extensions-']:
            if ext not in EXTENSIONS:
                raise Exception('Unrecognized extension \'' + ext + '\' in test header of test file \'' + path + '\'')
        if result['scenarios'] != [] and (result['compiler'] != [] or result['interpret'] != []):
            raise Exception('invalid combination of input and compiler/interpret pragma in test header of test file \'' + path + '\'')
        if result['compiler'] == []:
            result['compiler'].append(0)
        if result['interpret'] == []:
            result['interpret'].append(0)
        if result['code'] == '':
            raise Exception('No test code loaded from test file \'' + path + '\'')
        
        if result['scenarios'] != []:
            results = []
            for scenario in result['scenarios']:
                newTest = {}
                newTest['name'] = result['name'] + ':' + scenario[0]
                newTest['nogo'] = result['nogo']
                newTest['code'] = result['code']
                newTest['compiler'] = result['compiler']
                newTest['extensions+'] = result['extensions+']
                newTest['extensions-'] = result['extensions-']
                newTest['interpret'] = result['interpret']
                newTest['input'] = scenario[1]
                results.append(newTest)
        else:
            result['input'] = ''
            results = [result]

        return results

    result = []
    for item in args.select:
        if os.path.isfile(item) and item.endswith('.go'):
            result += ProcessTestFile(item)
        elif os.path.isdir(item):
            for directory, _, files in os.walk(item):
                for f in files:
                    if f.endswith('.go'):
                        result += ProcessTestFile(os.path.join(directory, f))
        else:
            raise Exception('\'' + item + '\' is not a test file or a test directory')

    for i in range(len(result)):
        for j in range(i+1, len(result)):
            split_a = result[i]['name'].split(':')
            split_b = result[j]['name'].split(':')
            if os.path.basename(split_a[0]) == os.path.basename(split_b[0]):
                if len(split_a) > 1 and len(split_b) > 1 and split_a[1] == split_b[1]:
                    raise Exception('Test files \'' + result[i]['name'] + '\' and \'' + result[j]['name'] + '\' have the same name')


    print('Total tests selected: \'' + str(len(result)) + '\'')
    return result

# Run test on native go interpreter
def RunGo(test_code, program_input, interpret, tmp_dir):
    global process
    # Execute test on native go interpreter
    tmp_file = os.path.join(tmp_dir, TMP_GO_FILE_NAME)
    template_file = os.path.join(tmp_dir, TMP_TEMPLATE_FILE_NAME)
    with open(tmp_file, 'w') as f:
        f.write(test_code)
    cmd = [interpret, 'run', template_file, tmp_file]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    capture_out, capture_err = process.communicate(input=program_input)
    return_code = process.returncode
    process = None
    # Return info about the execution
    return {'exit_code' : return_code,
            'stdout' : capture_out,
            'stderr' : capture_err}

# Run test on ifj20 compiler
def RunIfjcomp(test_code, compiler):
    global process
    # Execute test on ifj20 compiler
    process = subprocess.Popen(compiler, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    capture_out, capture_err = process.communicate(input=test_code)
    return_code = process.returncode
    process = None
    # Return info about the execution
    return {'exit_code' : return_code,
            'stdout' : capture_out,
            'stderr' : capture_err}

# Run interpret with intermediate code
def RunIclint(input_data, program_input, tmp_dir, interpret):
    global process
    # Save intermediate code to file
    tmp_file = os.path.join(tmp_dir, TMP_IFJCODE_FILE_NAME)
    with open(tmp_file, 'w') as f:
        f.write(input_data)
    # Execute code on interpreter
    cmd = [interpret, tmp_file]
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    capture_out, capture_err = process.communicate(input=program_input)
    return_code = process.returncode
    process = None
    # Return info about the execution
    return {'exit_code' : return_code,
            'stdout' : capture_out,
            'stderr' : capture_err}

# Check error code from ifj20 compiler
def CheckCompilerError(process_info, error_code):
    # Check for correct error code
    if process_info['exit_code'] not in error_code:
	# Log error
        Log('Compiler error output:\n' + (process_info['stderr'] or '<empty>'))
        Log('----')
        error = 'Unexpected exit code of IFJ compiler. Actual: ' + str(process_info['exit_code']) + ' Expected: ' + str(error_code) + '.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

# Check error code from ifj20 interpreter
def CheckInterpretError(process_info, error_code):
    if process_info['exit_code'] not in error_code:
	# Log error
        Log('Interpret error output:\n' + (process_info['stderr'] or '<empty>'))
        Log('----')
        error = 'Unexpected exit code of IFJ interpreter. Actual: ' + str(process_info['exit_code']) + ' Expected: ' + str(error_code) + '.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

# Check output from native go interpreter and ifj20 interpreter
def CheckSameOutput(interpret_info, go_info, log_success):
    def TransformOutput(output):
        return re.sub(r'(0x[0-9a-fA-F\.]+p\+)0([0-9])', r'\1\2', output)

    # Check if go and ifj20 have the save exit code
    if interpret_info['exit_code'] != go_info['exit_code']:
	# Log error
        Log('Go error output:\n' + (go_info['stderr'] or '<empty>'))
        Log('----')
        Log('Interpret error output:\n' + (interpret_info["stderr"] or '<empty>'))
        Log('----')
        error = 'Go and IFJ interprets have different exit codes. Go: ' + str(go_info['exit_code']) + ' IFJ: ' + str(interpret_info['exit_code']) + '.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_id + ' - ' + error)

    # Check standart output of go and ifj20
    if (interpret_info['stdout'] != go_info['stdout']) and (interpret_info['stdout'] != TransformOutput(go_info['stdout'])):
	# Log error
        Log('Go error output:\n' + (go_info['stderr'] or '<empty>'))
        Log('----')
        Log('Interpret error output:\n' + (interpret_info['stderr'] or '<empty>'))
        Log('----')
        Log('Go output:\n' + (go_info['stdout'] or '<empty>'))
        Log('----')
        Log('Interpret output:\n' + (interpret_info['stdout'] or '<empty>'))
        Log('----')
        error = 'Go and IFJ interprets have different outputs.'
        Log('ERROR: ' + error)
	# Fail test
        raise RuntimeError(test_iid + ' - ' + error)
    
    # Log successfull test output
    if log_success:
        Log('Interpret output:\n' + (interpret_info['stdout'] or '<empty>'))
        Log('----')

def CheckExtensions(required, forbiden, extensions):
    for item in required:
        if item not in extensions:
            return False
    for item in forbiden:
        if item in extensions:
            return False
    return True

# Save intermetiate code
def SaveIfjcode(name, directory, data):
        parts = name.split(':')
        fileName = os.path.basename(parts[0])
        if len(parts) > 1:
            fileName += ':' + os.path.basename(parts[1])
        outputFileName = fileName + '.ifjcode'
        f = open(os.path.join(directory, outputFileName), 'w')
        f.write(data)
        f.close()

def RunTest(test, args):
    # Global variables must be accessed here
    global test_index
    global test_id
    test_index = test_index + 1
    test_id = test['name']
    # Log current test
    Log('\n********************\nTEST ' + str(test_index) + ': ' + test_id + '\n********************\n')
    if test['input'] != '':
        Log('PROGRAM INPUT:')
        Log(test['input'])
        Log('----')
    # Check extensions
    if not CheckExtensions(test['extensions+'], test['extensions-'], args.extensions):
        Log('test skiped due to extensions missmatch')
        Log('Required extensions: ' ' '.join(test['extensions+']))
        Log('Forbiden extensions: ' ' '.join(test['extensions-']))
        Log('Implemented extensions: ' ' '.join(args.extensions))
        Log('----')
        Log('SKIPED')
        return False
    # Run ifj20 compiler
    compiler_info = RunIfjcomp(test['code'], args.compiler)
    # Check compiler for error
    CheckCompilerError(compiler_info, test['compiler'])
    # End execution if tests are compile only
    if args.mode_compile_only or (compiler_info['exit_code'] != 0):
        if args.mode_compile_only and (compiler_info['exit_code'] == 0):
	    # Log warning about incomplete testing
            Log('WARNING: This test was not entirely completed, because of the MODE_COMPILE testing mode.')
            Log('         Interpretation and output checks were not run.')
            Log('----')
        Log('SUCCESS')
        return True

    # If this part fails, the intermedate code must be saved for further analysis
    try:
	# Run ifj20 interpret
        interpret_info = RunIclint(compiler_info['stdout'], test['input'], args.tmp_dir, args.ifjcode_interpreter)
	# Check interpret for error
        CheckInterpretError(interpret_info, test['interpret'])
	# End execution if tests are compile and interpret only
        if args.mode_interpret_only or (interpret_info['exit_code'] != 0) or test['nogo']:
            if args.mode_interpret_only and (interpret_info['exit_code'] == 0) and not test['nogo']:
		# Log warning about incomplete testing
                Log('WARNING: This test was not entirely completed, because of the MODE_INTERPRET testing mode.')
                Log('         Output checks were not run.')
                Log('----')
            Log('SUCCESS')
            return True

	# Run test on native go interpreter and compare results with ifj20 interpreter
        go_info = RunGo(test['code'], test['input'], args.go_interpreter, args.tmp_dir)
        CheckSameOutput(interpret_info, go_info, args.log_success_output)
    except:
        SaveIfjcode(test['name'], args.output_folder, compiler_info['stdout'])
        raise
    else:
        if args.save_ifjcode_all:
            SaveIfjcode(test['name'], args.output_folder, compiler_info['stdout'])

    # Test successfull
    Log('SUCCESS')
    return True

def AlarmHandle(signum, frame):
    Log('ERROR: test timeout')
    raise RuntimeError("test timeout")



# Main program

args = ParseArgs()
tests = ProcessTests(args)
print('\n-------- RESULTS --------\n')
if args.log_none:
    logEnable = False
elif not args.log_output:
    log = open(args.log_file, 'w')
signal.signal(signal.SIGALRM, AlarmHandle)
passed = 0
failed = 0
skiped = 0
for test in tests:
    try:
        signal.alarm(args.timeout)
        result = RunTest(test, args)
        signal.alarm(0)
    except Exception as error:
        if process is not None:
            process.terminate()
        failed = failed + 1
        print(test['name'] + ': FAILED')
    else:
        if result:
            passed = passed + 1
            print(test['name'] + ': PASSED')
        else:
            skiped = skiped + 1
            print(test['name'] + ': SKIPED')
if log is not None:
    log.close()
if os.path.isdir(args.tmp_dir):
    shutil.rmtree(args.tmp_dir)

print('\n-------- SUMMARY --------\n')
print('PASSED: ' + str(passed))
print('FAILED: ' + str(failed))
print('SKIPED: ' + str(skiped))
