APPNAME = 'sais'
VERSION = '0.1.0'

top = '.'
out = 'build'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('unittestt')

def configure(cnf):
  cnf.load('compiler_cxx')
  cnf.load('unittestt')
  cnf.env.CXXFLAGS = ['-std=gnu++0x', '-g', '-W', '-Wall', '-O3']
  # cnf.env.CXXFLAGS = ['-std=gnu++0x', '-g', '-W', '-Wall', '-O0']

def build(bld):
  bld.recurse('src')
