from distutils.core import setup, Extension
setup(
    name='py_secrets',
    version='Alpha 0.01',
    description='Python C extension module for safe management\n' \
        + 'and removal of secrets and sensitive data in memory. ',
    author = 'the-waffle',
    ext_modules=[Extension('py_secrets', sources = ['py_secrets.c'])]
    )